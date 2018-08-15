#include "stdafx.h"
#include "downloader.h"

#include "helper.h"

bool Downloader::lastErrorWasLocalFault;

bool Downloader::getRemoteTextFile(std::string url, std::string *output)
{
	// Curl handles
	CURL *curl = curl_easy_init();
	if (curl)
	{
		CurlCustomProgressData progressData;
		progressData.progressVariable = nullptr;
		progressData.totalProgressVariable = nullptr;
		progressData.lastTransferTime = Helper::currentTimeMillis();

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteToString);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, output);
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, curlTransferProgress);
		curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &progressData);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		CURLcode result = curl_easy_perform(curl);

		// Get http status code
		long httpStatusCode = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatusCode);

		// Check it
		if (result == CURLE_OK && httpStatusCode == 200)
		{
			// Success
			curl_easy_cleanup(curl);
			return true;
		}
	}

	// Failed
	curl_easy_cleanup(curl);
	return false;
}

bool Downloader::downloadBinaryFile(std::string url, std::wstring output, int *progress, int *totalProgress)
{
	CURL *curl = curl_easy_init();
	if (curl)
	{
		// Try to open the local file for writing
		Helper::createDirectoriesForPath(output); // try and create directory before in case it does not exist
		FILE *f = NULL;
		_wfopen_s(&f, output.c_str(), L"wb");
		if (f == NULL)
		{
			wchar_t test[512];
			_snwprintf_s(test, 511, L"%d", errno);

			// failed to open local file
			curl_easy_cleanup(curl);
			lastErrorWasLocalFault = true;
			return false;
		}

		// Custom data to pass to curl write function
		CurlCustomWriteData writeData;
		writeData.file = f;
		CurlCustomProgressData progressData;
		progressData.progressVariable = progress;
		progressData.totalProgressVariable = totalProgress;
		progressData.lastTransferTime = Helper::currentTimeMillis();

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteToFile);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &writeData);
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, curlTransferProgress);
		curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &progressData);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		CURLcode result = curl_easy_perform(curl);

		// get http status code
		long httpStatusCode = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatusCode);

		// check it
		if (result == CURLE_OK && httpStatusCode == 200)
		{
			// success
			fclose(f);
			curl_easy_cleanup(curl);
			return true;
		}

		fclose(f);
		//MessageBox(NULL, L"Failed to download file", output.c_str(), 0);
	}

	// failed
	curl_easy_cleanup(curl);
	lastErrorWasLocalFault = false;
	return false;
}

size_t Downloader::curlWriteToString(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*) userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

size_t Downloader::curlWriteToFile(void *contents, size_t size, size_t nmemb, void *userp)
{
	// Cast userp pointer to struct
	CurlCustomWriteData *data = (CurlCustomWriteData *)userp;
	FILE *f = data->file;

	size_t written = fwrite(contents, size, nmemb, f);
	return written;
}

int Downloader::curlTransferProgress(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
	// Cast p pointer to struct
	CurlCustomProgressData *data = (CurlCustomProgressData *)p;

	// Mark last transfer time and check for timeout
	int transferedMeanwhile = dlnow - data->lastProgress;
	if (transferedMeanwhile > 0)
		data->lastTransferTime = Helper::currentTimeMillis();
	if (Helper::currentTimeMillis() - data->lastTransferTime > 60 * 1000)
		return 1;
	data->lastProgress = dlnow;

	// Increment progress variables if they were provided
	int *progressVariable = data->progressVariable;
	if (progressVariable != nullptr) *progressVariable = dlnow;
	int *totalProgressVariable = data->totalProgressVariable;
	if (totalProgressVariable != nullptr) *totalProgressVariable += transferedMeanwhile;
	return 0;
}