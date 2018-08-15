#include "stdafx.h"
#include "compression.h"

#include <zlib.h>

#include "constants.h"

bool Compression::decompressGzipFile(std::wstring compressedPath, std::wstring decompressedPath)
{
	// open .gz file
	FILE *decompressedFile = NULL;
	_wfopen_s(&decompressedFile, decompressedPath.c_str(), L"wb");

	if (decompressedFile == NULL)
	{
		//MessageBox(NULL, decompressedPath.c_str(), L"Failed to open file for writing", 0);
		return false;
	}

	unsigned char buffer[Constants::FILE_BUFFER_SIZE];
	gzFile compressedFile = gzopen_w(compressedPath.c_str(), "rb");
	
	if (compressedFile == NULL)
	{
		fclose(decompressedFile);
		//MessageBox(NULL, decompressedPath.c_str(), L"Failed to open file for reading", 0);
		return false;
	}

	gzrewind(compressedFile);
	while (!gzeof(compressedFile))
	{
		int len = gzread(compressedFile, buffer, sizeof(buffer));
		fwrite(buffer, 1, len, decompressedFile);
	}

	gzclose(compressedFile);
	fclose(decompressedFile);
	return true;
}