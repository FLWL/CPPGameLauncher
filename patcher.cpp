#include "stdafx.h"
#include "patcher.h"

#include <fstream>
#include <sstream>
#include <streambuf>
#include <algorithm>
#include <iomanip>  // Includes ::std::hex
#include <iostream> // Includes ::std::cout
#include <chrono>
#include <thread>

#include "constants.h"
#include "crc.h"
#include "sha256.h"
#include "downloader.h"
#include "rsaverifier.h"
#include "compression.h"
#include "iniparser.h"

Patcher::Patcher()
{
	// For communicating with UI
	fileDone = 0;
	fileTotal = 0;
	filesDone = 0;
	filesTotal = 0;
	totalBytesDone = 0;
	totalBytesTotal = 0;
	currentFileName = L"";
	newsFile = L"";

	// Set state
	statusText = L"Initializing...";
	smallStatusText = L"";
	state = STATE_STARTING;
	paused = false;
	continueAfterPause = false;
	pauseMessage = L"";
	ignoreLocalVersionFile = false;

	// Clean up old launcher version if necessary
	std::wstring oldVersionPath = Helper::getExecutableDirectory() + L"/old_launcher.bak";
	_wremove(oldVersionPath.c_str());
}

void Patcher::run()
{
	bool looping = true;
	for (int i = 1; i <= Constants::NUMBER_OF_UPDATE_SERVERS; i++)
	{
		if (!looping) break;
		std::string updateServer = "update" + i + Constants::UPDATE_DOMAIN;
		bool lastServer = i == Constants::NUMBER_OF_UPDATE_SERVERS;

		if (tryFromServer(updateServer, lastServer))
		{
			// success
			break;
		}

		// failure, check what state the patcher was left in and take appropriate action
		switch (state)
		{
			case PatcherState::STATE_LOADING_REMOTE_VERSION_FAILED:
			{
				// loading remote version can only fail in case of:
				// a) no connection to server
				// b) invalid version file
				// can't be our fault, try the next server
				continue;
			}
			case PatcherState::STATE_LOADING_REMOTE_CHECKSUMS_FAILED:
			{
				// loading checksums can only fail in case of:
				// a) no connection to the server
				// b) or the server gave us an invalid version number/signature
				// can't be a local fault, therefore try another server
				continue;
			}
			case PatcherState::STATE_CHECKING_FILES_FAILED:
			{
				// failed to check local files, this is definitely our fault
				// (or the server checksums file contained illegal paths)
				// plus user chose to not continue with the patching process,
				// and try again instead
				if (lastServer) i = 1; // reset if last server, because user wanted to try again
				continue;
			}
			case PatcherState::STATE_DOWNLOADING_FILES_FAILED_WITH_LOCAL_ISSUES:
			{
				// local issues prevented us from downloading all files
				// stop trying
				looping = false;
				break;
			}
			case PatcherState::STATE_DOWNLOADING_FILES_FAILED:
			{
				// remote issues stopped us from downloading
				// try another server
				continue;
			}
		}
	}

	if (looping && state != PatcherState::STATE_SUCCESS)
	{
		// all update servers were exhausted, but there is still something wrong

	}
}

bool Patcher::tryFromServer(std::string server, bool lastServer)
{
	// Remote directory
	std::wstring localDirectory = Helper::getExecutableDirectory();
	std::string remoteDirectory = server + Constants::UPDATE_DIRECTORY;

	// Before anything, get news if it hasn't been done yet
	if (newsFile.empty())
		newsFile = getNews(remoteDirectory);

	// Get installed and latest versions
	int installedVersion = getInstalledVersion(localDirectory);
	int latestVersion, latestLauncherVersion, numberOfUpdateServers;
	std::string latestLauncherChecksum;
	getLatestVersion(remoteDirectory, latestVersion, latestLauncherVersion, latestLauncherChecksum, numberOfUpdateServers);

	// Check if launcher needs updating
	if (isValidVersion(latestLauncherVersion) && latestLauncherVersion > Constants::LAUNCHER_VERSION)
	{
		smallStatusText = L"Updating your launcher to v" + Helper::versionNumberToString(latestLauncherVersion);

		if (updateLauncher(localDirectory, remoteDirectory, latestLauncherChecksum))
		{
			// updating was a success, and since we are going to exit anyway, mark our patcher as done
			state = STATE_SUCCESS;
			statusText = L"Launcher version has been updated. Please restart the application.";
			return true;
		}
	}

	// Check if remote game version is invalid
	if (!isValidVersion(latestVersion))
	{
		state = STATE_LOADING_REMOTE_VERSION_FAILED;
		statusText = L"Could not connect to update server!";
		return false; // server does not have a valid version file
	}

	// Compare the local version against remote
	if (!ignoreLocalVersionFile && installedVersion >= latestVersion)
	{
		state = STATE_SUCCESS;
		statusText = L"Latest version already installed!";
		return true;
	}

	// Versions are different, update. Mark local installation as not latest. First fetch remote checksums file
	writeVersionFile(localDirectory, 0);
	std::string remotePatchDirectory = remoteDirectory + "/" + std::to_string(latestVersion);
	std::string remoteChecksums = getRemoteChecksumFile(remotePatchDirectory);
	std::string remoteChecksumsSignature = getRemoteChecksumSignatureFile(remotePatchDirectory);

	if (remoteChecksums.empty())
	{
		state = STATE_LOADING_REMOTE_CHECKSUMS_FAILED;
		statusText = L"The game was not updated and might not work properly:\nLoading patch info failed!";
		return false;
	}
	if (remoteChecksumsSignature.empty())
	{
		state = STATE_LOADING_REMOTE_CHECKSUMS_FAILED;
		statusText = L"The game was not updated and might not work properly:\nLoading patch info certificate failed!";
		return false;
	}

	// Verify downloaded checksums file
	if (!RSAVerifier::verifyData(remoteChecksums, remoteChecksumsSignature))
	{
		state = STATE_LOADING_REMOTE_CHECKSUMS_FAILED;
		statusText = L"The game was not updated and might not work properly:\nPatch info certificate verification failed!";
		return false;
	}

	// Parse downloaded checksums file
	std::vector<PatchableFile> remoteFiles = parseChecksums(remoteChecksums);
	updateUninstallFile(localDirectory, remoteFiles);

	if (remoteFiles.size() == 0)
	{
		state = STATE_LOADING_REMOTE_CHECKSUMS_FAILED;
		statusText = L"The game was not updated and might not work properly:\nCorrupted patch info file!";
		return false;
	}

	// Determine which local files need to be updated
	int filesFailed, filesToDownload, bytesToDownload;
	checkLocalFiles(localDirectory, remoteFiles, filesFailed, filesToDownload, bytesToDownload);

	if (filesFailed)
	{
		// files can fail if they existed but we failed to open them
		// ask the user if they want to ignore failed files or retry
		if (!continueWithFailedFiles(remoteFiles, L"Following files could not be checked:\n"))
		{
			state = STATE_CHECKING_FILES_FAILED;
			statusText = L"The game was not updated and might not work properly:\nSome local files could not be checked!";
			return false;
		}
	}
	
	if (filesToDownload == 0)
	{
		// all files up to date, update version file
		if (!filesFailed)
		{
			state = STATE_SUCCESS;
			statusText = L"All files have been checked and are up to date!";
			writeVersionFile(localDirectory, latestVersion);
		}
		else
		{
			state = STATE_CHECKING_FILES_FAILED;
			statusText = L"The game was not updated and might not work properly:\nNo local files to update, but some could not be checked!";
			return false;
		}
	}

	// Download necessary files
	int failedFiles, localFaults;
	downloadFiles(localDirectory, remotePatchDirectory, remoteFiles, filesToDownload, bytesToDownload, failedFiles, localFaults);
	
	if (localFaults)
	{
		// some local issues were detected, notify the user
		continueWithFailedFiles(remoteFiles, L"Following files could not be updated:\n");
		state = STATE_DOWNLOADING_FILES_FAILED_WITH_LOCAL_ISSUES;
		statusText = L"The game was not updated and might not work properly:\nSome files had local issues. They might be in use or you don't have access to them.";
		return false;
	}
	else if (failedFiles)
	{
		// no local issues, but some files still failed
		if (lastServer) continueWithFailedFiles(remoteFiles, L"Following files could not be updated:\n");
		state = STATE_DOWNLOADING_FILES_FAILED;
		statusText = L"The game was not updated and might not work properly:\nSome files had download issues.";
		return false;
	}

	// all files were successfully downloaded, mark the installation as up-to-date
	state = STATE_SUCCESS;
	statusText = L"All files have been successfully updated!";
	writeVersionFile(localDirectory, latestVersion);
	return true;
}

int Patcher::getInstalledVersion(std::wstring installationDirectory)
{
	// Set state
	state = STATE_CHECKING_LOCAL_VERSION;
	statusText = L"Determining installed version...";

	// Read version file
	std::ifstream t(installationDirectory + Constants::VERSION_FILE);
	std::string szLocalPatchVersion = std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	t.close();

	// Convert to int safely
	return Helper::stringToInt(szLocalPatchVersion);
}

bool Patcher::getLatestVersion(std::string remoteDirectory, int &latestGameVersion, int &latestLauncherVersion, std::string &latestLauncherChecksum, int &numberOfUpdateServers)
{
	// Set state
	state = STATE_LOADING_REMOTE_VERSION;
	statusText = L"Checking for updates...";

	// Download latest version file from server
	std::string szLatestVersion;
	if (!Downloader::getRemoteTextFile(remoteDirectory + Constants::REMOTE_VERSION_FILE, &szLatestVersion))
		return false;

	// Download a signature file
	std::string szLatestVersionSignature;
	if (!Downloader::getRemoteTextFile(remoteDirectory + Constants::REMOTE_VERSION_SIGNATURE_FILE, &szLatestVersionSignature))
		return false;

	// Check signature
	if (!RSAVerifier::verifyData(szLatestVersion, szLatestVersionSignature))
		return false;

	// Parse the file
	latestGameVersion = 0;
	latestLauncherVersion = 0;
	latestLauncherChecksum = "";
	numberOfUpdateServers = 0;

	std::stringstream ss;
	ss << szLatestVersion;

	try
	{
		INI::Parser p(ss);
		latestGameVersion = Helper::stringToInt(p.top()["game_version"]);
		latestLauncherVersion = Helper::stringToInt(p.top()["launcher_version"]);
		latestLauncherChecksum = p.top()["launcher_checksum"];
		fileTotal = Helper::stringToInt(p.top()["launcher_size"]);
		currentFileName = L"Launcher Update";
		numberOfUpdateServers = Helper::stringToInt(p.top()["update_servers"]);
	}
	catch (std::runtime_error &e)
	{
		return false;
	}

	return true;
}

bool Patcher::updateLauncher(std::wstring localDirectory, std::string remoteDirectory, std::string latestLauncherChecksum)
{
	// Set state
	state = STATE_UPDATING_LAUNCHER;
	statusText = L"";

	// Download the latest launcher executable
	std::wstring localPath = localDirectory + L"/latest_launcher.download";
	std::string remotePath = remoteDirectory + Constants::REMOTE_LAUNCHER_EXECUTABLE_FILE;

	if (Downloader::downloadBinaryFile(remotePath, localPath, &fileDone, nullptr))
	{
		std::string sha256 = SHA256::getFileHash(localPath);

		if (sha256 == latestLauncherChecksum)
		{
			// new file is good, rename our current launcher executable
			std::wstring currentExecutable = Helper::getExecutablePath();
			std::wstring renamedCurrentExecutable = localDirectory + L"/old_launcher.bak";
			if (_wrename(currentExecutable.c_str(), renamedCurrentExecutable.c_str()) == 0)
			{
				// renamed CPPGameLauncher.exe -> old_launcher.bak

				if (_wrename(localPath.c_str(), currentExecutable.c_str()) == 0)
				{
					// renamed latest_launcher.exe -> CPPGameLauncher.exe
					// try to launch the newly acquired exe
					if (Helper::executeProgram(currentExecutable, L"-ignoreinstances"))
					{
						std::exit(0);
						return true;
					}
					else
					{
						// launching of the new exe failed, revert everything
						_wremove(currentExecutable.c_str());
						_wrename(renamedCurrentExecutable.c_str(), currentExecutable.c_str());
					}
				}
				else
				{
					// could not rename downloaded exe into CPPGameLauncher.exe
					// rename old_launcher.bak -> CPPGameLauncher.exe
					_wremove(localPath.c_str());
					_wrename(renamedCurrentExecutable.c_str(), currentExecutable.c_str());
				}
			}
			else
			{
				// current exe rename failed, delete our downloaded launcher
				_wremove(localPath.c_str());
			}
		}
		else
		{
			// sha256 verification failed, get rid of the downloaded exe
			_wremove(localPath.c_str());
		}
	}

	return false;
}

std::string Patcher::getRemoteChecksumFile(std::string remotePatchDirectory)
{
	// Set state
	state = STATE_DOWNLOADING_REMOTE_CHECKSUMS;
	statusText = L"Downloading latest patch info...";

	std::string szRemoteChecksums;
	if (!Downloader::getRemoteTextFile(remotePatchDirectory + Constants::REMOTE_CHECKSUMS_FILE, &szRemoteChecksums))
		return "";

	return szRemoteChecksums;
}

std::string Patcher::getRemoteChecksumSignatureFile(std::string remotePatchDirectory)
{
	// Set state
	state = STATE_DOWNLOADING_REMOTE_CHECKSUMS;
	statusText = L"Downloading latest patch info...";

	std::string szRemoteChecksums;
	if (!Downloader::getRemoteTextFile(remotePatchDirectory + Constants::REMOTE_CHECKSUMS_SIGNATURE_FILE, &szRemoteChecksums))
		return "";

	return szRemoteChecksums;
}

std::vector<PatchableFile> Patcher::parseChecksums(std::string checksums)
{
	std::vector<PatchableFile> remoteFiles;

	totalBytesDone = 0;
	std::istringstream iss(checksums);
	std::string line;

	while (std::getline(iss, line))
	{
		PatchableFile file;

		std::vector<std::string> splitLine = Helper::split(line, '\t');
		file.name = splitLine[0];
		file.size = Helper::stringToInt(splitLine[1]);
		file.compressedSize = Helper::stringToInt(splitLine[2]);
		file.crc = splitLine[3];
		transform(file.crc.begin(), file.crc.end(), file.crc.begin(), ::toupper); // convert crc to uppercase
		file.sha256 = splitLine[4].substr(0, 64);

		// keep track of the sum of all filesizes
		totalBytesTotal += file.size;
		remoteFiles.push_back(file);
	}

	return remoteFiles;
}

void Patcher::checkLocalFiles(std::wstring localDirectory, std::vector<PatchableFile> &remoteFiles, int &filesFailed, int &filesToDownload, int &bytesToDownload)
{
	// Set state
	filesDone = 0;
	filesTotal = remoteFiles.size();
	totalBytesDone = 0;
	state = STATE_CALCULATING_LOCAL_CHECKSUMS;
	statusText = L"";
	filesFailed = 0;
	filesToDownload = 0;
	bytesToDownload = 0;

	CRC::Table<std::uint32_t, 32> table(CRC::CRC_32()); // precompute crc table
	unsigned char buffer[Constants::FILE_BUFFER_SIZE];

	for (int i = 0; i < filesTotal; i++)
	{
		// Set state
		fileDone = 0;
		fileTotal = 100;

		PatchableFile *currentFile = &remoteFiles[i];
		currentFileName = Helper::s2ws(currentFile->name);
		//currentFileName = currentFileName.substr(currentFileName.find_last_of(L"\\/") + 1, currentFileName.length());
		std::wstring localFilePath = localDirectory + L"\\" + Helper::s2ws(currentFile->name);

		// Check if the local file exists
		if (!Helper::fileExists(localFilePath))
		{
			//MessageBox(NULL, L"File does not exist, patching!", localFilePath.c_str(), 0);
			// does not exist, add to list of files to download
			currentFile->download = true;
			filesToDownload++;
			bytesToDownload += currentFile->compressedSize;
			fileDone = 100;
			continue;
		}

		// File exists, try to open it
		FILE *f = NULL;
		_wfopen_s(&f, localFilePath.c_str(), L"rb");
		if (f == NULL)
		{
			// failed to open local file
			//MessageBox(NULL, L"Failed to open local file, despite it existing!", L"Patching error", 0);
			currentFile->download = false;
			currentFile->failed = true;
			currentFile->failReason = L"failed to open for checking";
			filesFailed++;
			continue;
		}

		// File successfully opened, get size
		fseek(f, 0, SEEK_END);
		int localFileSize = (int)ftell(f);
		rewind(f);

		// Compare against remote size if applicable
		if (localFileSize != currentFile->size)
		{
			//MessageBox(NULL, L"File size wrong, patching!", localFilePath.c_str(), 0);
			fclose(f);
			currentFile->download = true;
			filesToDownload++;
			bytesToDownload += currentFile->compressedSize;
			fileDone = 100;
			continue;
		}

		// File sizes match, read local file contents and calculate its CRC
		fileTotal = localFileSize;
		size_t bytesRead = 0;
		std::uint32_t crc = NULL;
		// read up to sizeof(buffer) bytes
		while ((bytesRead = fread(buffer, 1, sizeof(buffer), f)) > 0)
		{
			// process bytesRead worth of data in buffer
			crc = CRC::Calculate(buffer, bytesRead, table, crc);
			fileDone += bytesRead;
			totalBytesDone += bytesRead;
		}

		// Convert our calculated CRC to hex
		std::stringstream stream;
		stream << std::setw(8) << std::setfill('0') << std::hex << crc;
		std::string crcHex = stream.str();
		transform(crcHex.begin(), crcHex.end(), crcHex.begin(), ::toupper); // convert to uppercase

		// Compare with the remote checksum
		if (currentFile->crc != crcHex)
		{
			// checksums don't match, mark this file to be updated
			//MessageBox(NULL, L"Checksums dont match, patching!", localFilePath.c_str(), 0);
			//MessageBoxA(NULL, crcHex.c_str(), remoteChecksums[i].c_str(), 0);
			currentFile->download = true;
			filesToDownload++;
			bytesToDownload += currentFile->compressedSize;
			fileDone = 100;
		}

		// close the file at the end
		fclose(f);
		currentFile->download = false;
		filesDone++;
	}
}

void Patcher::downloadFiles(std::wstring localDirectory, std::string remotePatchDirectory, std::vector<PatchableFile> &remoteFiles, int filesToDownload, int bytesToDownload, int &failedFiles, int &localFaults)
{
	// Set state
	filesDone = 0;
	filesTotal = filesToDownload;
	totalBytesDone = 0;
	totalBytesTotal = bytesToDownload;
	state = STATE_DOWNLOADING_REMOTE_FILES;
	failedFiles = 0;
	localFaults = 0;

	for (int i = 0; i < remoteFiles.size(); i++)
	{
		PatchableFile *currentFile = &remoteFiles[i];
		currentFile->failed = false; // reset previously failed files
		if (!currentFile->download)
			continue;

		// Set state
		fileDone = 0;
		fileTotal = currentFile->compressedSize;
		currentFileName = Helper::s2ws(currentFile->name);

		std::string remoteUrl = remotePatchDirectory + "/files/" + currentFile->name + ".gz";
		std::wstring compressedPath = localDirectory + L"\\" + Helper::s2ws(currentFile->name) + L".gz";
		std::wstring decompressedPath = localDirectory + L"\\" + Helper::s2ws(currentFile->name);
		if (Downloader::downloadBinaryFile(remoteUrl, compressedPath, &fileDone, &totalBytesDone))
		{
			std::string sha = SHA256::getFileHash(compressedPath);

			if (sha.size() != 64)
			{
				currentFile->failed = true;
				currentFile->failReason = L"failed to open for verification";
				failedFiles++; // failed to open file for sha256 calculation
				localFaults++;
				continue;
			}

			if (sha == currentFile->sha256)
			{
				// decompress downloaded .gz file
				if (!Compression::decompressGzipFile(compressedPath, decompressedPath))
				{
					//MessageBox(NULL, compressedPath.c_str(), L"file decompression failed", 0);
					currentFile->failed = true;
					currentFile->failReason = L"failed to decompress";
					failedFiles++;
					localFaults++; // decompression can only fail due to local faults, assuming the .gz file is valid (as we checked the hash)
					continue;
				}
				// delete .gz file
				_wremove(compressedPath.c_str());
			}
			else
			{
				// verification failed, server fault
				//MessageBox(NULL, compressedPath.c_str(), L"SHA256 verification failed", 0);
				currentFile->failed = true;
				currentFile->failReason = L"verification failed";
				failedFiles++;
				continue;
			}

			//MessageBox(NULL, outputPath.c_str(), L"successfully downloaded", 0);
			filesDone++;
		}
		else
		{
			currentFile->failed = true;
			currentFile->failReason = L"downloading unsuccessful";
			failedFiles++; // downloading failed, take the next file on the next loop
			if (Downloader::lastErrorWasLocalFault) localFaults++;
		}
	}
}

std::wstring Patcher::getNews(std::string remoteDirectory)
{
	std::wstring news;
	std::string tempNewsFile, tempNewsSignature;
	if (Downloader::getRemoteTextFile(remoteDirectory + Constants::REMOTE_NEWS_FILE, &tempNewsFile))
	{
		if (Downloader::getRemoteTextFile(remoteDirectory + Constants::REMOTE_NEWS_SIGNATURE_FILE, &tempNewsSignature))
		{
			if (RSAVerifier::verifyData(tempNewsFile, tempNewsSignature))
			{
				news = Helper::s2ws(tempNewsFile);
			}
		}
	}

	return news;
}

bool Patcher::isValidVersion(int version)
{
	return version > 0;
}

void Patcher::writeVersionFile(std::wstring localDirectory, int newVersion)
{
	std::ofstream out(localDirectory + Constants::VERSION_FILE);
	out << newVersion;
	out.close();
}

void Patcher::updateUninstallFile(std::wstring localDirectory, std::vector<PatchableFile> &downloadedFiles)
{
	// Read the uninstall data file
	std::ifstream file(localDirectory + Constants::UNINSTALL_FILE);
	std::vector<std::string> fileLines;
	for (std::string line; std::getline(file, line); /**/)
		fileLines.push_back(line);

	// Open file for writing
	file.close();
	std::ofstream out(localDirectory + Constants::UNINSTALL_FILE, std::ios_base::app | std::ios_base::out);

	// Loop through downloaded files
	for (int i = 0; i < downloadedFiles.size(); i++)
	{
		PatchableFile *currentFile = &downloadedFiles[i];

		if (/*currentFile->download && !currentFile->failed*/ true)
		{
			// this file was downloaded, add it to list if it already isn't

			bool isInFile = false;
			for (int i = 0; i < fileLines.size(); i++)
			{
				if (fileLines[i] == currentFile->name)
				{
					isInFile = true;
					break;
				}
			}

			if (!isInFile)
			{
				out << currentFile->name << '\n';
			}
		}
	}

	out.close();
}

void Patcher::uninstallFiles(std::wstring localDirectory)
{
	// Read uninstall file
	std::ifstream file(localDirectory + Constants::UNINSTALL_FILE);
	std::string szUninstallFile = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	// Parse it line by line
	bool anyErrors = false;
	std::istringstream iss(szUninstallFile);
	std::string line;
	while (std::getline(iss, line))
	{
		std::wstring fullFilePath = localDirectory + L"\\" + Helper::s2ws(line);
		std::wstring compressedFilePath = localDirectory + L"\\" + Helper::s2ws(line) + L".gz";

		_wremove(fullFilePath.c_str());
		_wremove(compressedFilePath.c_str());

		// Try to delete the directory this file was in (fails if not empty)
		//unsigned int firstSlashPos = line.find_first_of("\\/");
		unsigned int pos = line.length();
		while (true)
		{
			pos = line.find_last_of("\\/", pos - 1);
			if (pos == std::wstring::npos)
				break;

			std::wstring dirToRemove = localDirectory + L"\\" + Helper::s2ws(line).substr(0, pos);
			//MessageBox(NULL, dirToRemove.c_str(), fullFilePath.c_str(), 0);
			RemoveDirectory(dirToRemove.c_str());
		}
	}

	// Remove some special files
	_wremove((localDirectory + L"\\" + Constants::VERSION_FILE).c_str());
	_wremove((localDirectory + L"\\" + Constants::UNINSTALL_FILE).c_str());
}

bool Patcher::continueWithFailedFiles(std::vector<PatchableFile> files, std::wstring message)
{
	// Check if any files failed and display a prompt to user
	std::wstring prompt = message;

	int filesDisplayed = 0;
	int failedFiles = 0;
	for (int i = 0; i < files.size(); i++)
	{
		PatchableFile *currentFile = &files[i];

		if (currentFile->failed)
		{
			if (filesDisplayed < 3)
			{
				prompt += L"\n" + Helper::s2ws(currentFile->name) + L" (Error: " + currentFile->failReason + L")";
				filesDisplayed++;
			}

			failedFiles++;
		}
	}

	for (int i = filesDisplayed; i < 3; i++)
	{
		prompt += L"\n";
	}

	if (filesDisplayed < failedFiles)
	{
		prompt += L"\n\n... and " + std::to_wstring(failedFiles - filesDisplayed) + L" more.";
	}

	promptForAdminAccess(prompt);
	return continueAfterPause;
}

void Patcher::promptForAdminAccess(std::wstring reason)
{
	pauseMessage = reason;
	paused = true;

	waitUntilUnpaused();
}

void Patcher::waitUntilUnpaused()
{
	while (paused)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}