#pragma once

#include <string>

class Compression
{
public:
	static bool decompressGzipFile(std::wstring compressedPath, std::wstring decompressedPath);
};