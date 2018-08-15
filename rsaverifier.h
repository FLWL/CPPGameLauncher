#pragma once

#include <vector>

#include "openssl/rsa.h"

class RSAVerifier
{
public:
	static bool verifyData(std::string data, std::string signature);
	static bool verifyData(std::vector<unsigned char> data, std::vector<unsigned char> signature, std::string publicKey);
private:
	static RSA *getRsaObjectFromPublicKey(std::string publicKey);
};