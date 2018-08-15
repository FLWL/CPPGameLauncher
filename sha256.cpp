#include "stdafx.h"
#include "sha256.h"

#include <openssl/sha.h>

#include "constants.h"

std::string SHA256::getFileHash(std::wstring filePath)
{
	// Open file for reading in binary mode
	FILE *f = NULL;
	_wfopen_s(&f, filePath.c_str(), L"rb");
	if (f == NULL)
		return "";

	// Initialize OpenSSL SHA256 verification context
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256Context;
	SHA256_Init(&sha256Context);

	// Feed file into the context part by part
	unsigned char buffer[Constants::FILE_BUFFER_SIZE];
	size_t bytesRead = 0;
	while ((bytesRead = fread(buffer, 1, sizeof(buffer), f)) > 0)
	{
		SHA256_Update(&sha256Context, buffer, bytesRead);
	}
	SHA256_Final(hash, &sha256Context);
	fclose(f);

	// Convert the resulting hash into a readable format
	std::string result = "";
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		char hexString[8];
		snprintf(hexString, sizeof(hexString) - 1, "%02x", hash[i]);
		result.append(hexString);
	}

	return result;
}