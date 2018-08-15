#pragma once

#include <string>
#include <vector>

#include "helper.h"

struct PatchableFile
{
	std::string name;
	int size;
	int compressedSize;
	std::string crc;
	std::string sha256;
	bool download;
	bool failed;
	std::wstring failReason;
};

class Patcher
{
public:
	Patcher();
	void run();
	static void uninstallFiles(std::wstring localDirectory);

	// Communicate with the UI
	int fileDone;
	int fileTotal;
	int filesDone;
	int filesTotal;
	int totalBytesDone;
	int totalBytesTotal;
	std::wstring currentFileName;
	std::wstring statusText;
	std::wstring smallStatusText;
	std::wstring newsFile;
	bool paused;
	bool continueAfterPause;
	std::wstring pauseMessage;
	bool ignoreLocalVersionFile;

	enum PatcherState
	{
		STATE_STARTING,
		STATE_CHECKING_LOCAL_VERSION,
		STATE_LOADING_REMOTE_VERSION,
		STATE_UPDATING_LAUNCHER,
		STATE_DOWNLOADING_REMOTE_CHECKSUMS,
		STATE_CALCULATING_LOCAL_CHECKSUMS,
		STATE_DOWNLOADING_REMOTE_FILES,
		STATE_SUCCESS,

		STATE_LOADING_REMOTE_VERSION_FAILED,
		STATE_LOADING_REMOTE_CHECKSUMS_FAILED,
		STATE_CHECKING_FILES_FAILED,
		STATE_DOWNLOADING_FILES_FAILED,
		STATE_DOWNLOADING_FILES_FAILED_WITH_LOCAL_ISSUES,
	};
	PatcherState state;
private:
	bool tryFromServer(std::string server, bool lastServer);
	int getInstalledVersion(std::wstring installationDirectory);
	bool getLatestVersion(std::string remoteDirectory, int &latestGameVersion, int &latestLauncherVersion, std::string &latestLauncherChecksum, int &numberOfUpdateServers);
	bool updateLauncher(std::wstring localDirectory, std::string remoteDirectory, std::string latestLauncherChecksum);
	std::string getRemoteChecksumFile(std::string remotePatchDirectory);
	std::string getRemoteChecksumSignatureFile(std::string remotePatchDirectory);
	std::vector<PatchableFile> parseChecksums(std::string checksums);
	void checkLocalFiles(std::wstring localDirectory, std::vector<PatchableFile> &remoteFiles, int &filesFailed, int &filesToDownload, int &bytesToDownload);
	void downloadFiles(std::wstring localDirectory, std::string remotePatchDirectory, std::vector<PatchableFile> &remoteFiles, int filesToDownload, int bytesToDownload, int &failedFiles, int &localFaults);
	std::wstring getNews(std::string remoteDirectory);

	bool isValidVersion(int version);
	void writeVersionFile(std::wstring localDirectory, int newVersion);
	void updateUninstallFile(std::wstring localDirectory, std::vector<PatchableFile> &downloadedFiles);
	bool continueWithFailedFiles(std::vector<PatchableFile> files, std::wstring message);
	void promptForAdminAccess(std::wstring message);
	void waitUntilUnpaused();
};
