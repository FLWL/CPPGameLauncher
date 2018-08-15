#pragma once

#include <string>

class SHA256
{
public:
	static std::string getFileHash(std::wstring path);
};