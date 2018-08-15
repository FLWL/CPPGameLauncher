#pragma once

#include <string>
#define CURL_STATICLIB
#include <curl/curl.h>

class Downloader
{
public:
	static bool getRemoteTextFile(std::string url, std::string *output);
	static bool downloadBinaryFile(std::string url, std::wstring output, int *progress, int *totalProgress);

	static bool lastErrorWasLocalFault; // not very thread safe
private:
	static size_t curlWriteToString(void *contents, size_t size, size_t nmemb, void *userp);
	static size_t curlWriteToFile(void *contents, size_t size, size_t nmemb, void *userp);
	static int curlTransferProgress(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);

	struct CurlCustomWriteData
	{
		FILE *file;
	};
	struct CurlCustomProgressData
	{
		int *progressVariable;
		int *totalProgressVariable;
		int lastProgress = 0;
		long long lastTransferTime;
	};
};