#include "stdafx.h"
#include "rsaverifier.h"

#include "openssl/pem.h"
#include "openssl/evp.h"

#include "constants.h"

bool RSAVerifier::verifyData(std::string data, std::string signature)
{
	std::vector<unsigned char> messageVector(data.begin(), data.end());
	std::vector<unsigned char> signatureVector(signature.begin(), signature.end());

	return verifyData(messageVector, signatureVector, Constants::MAIN_PUBLIC_KEY)
		|| verifyData(messageVector, signatureVector, Constants::AUXILIARY_PUBLIC_KEY);
}

bool RSAVerifier::verifyData(std::vector<unsigned char> data, std::vector<unsigned char> signature, std::string publicKey)
{
	RSA *rsa = getRsaObjectFromPublicKey(publicKey);

	if (rsa == NULL)
		return false;

	EVP_PKEY* publicKeyObject = EVP_PKEY_new();
	EVP_PKEY_assign_RSA(publicKeyObject, rsa);
	EVP_MD_CTX* verifyingContext = EVP_MD_CTX_create();

	if (EVP_DigestVerifyInit(verifyingContext, NULL, EVP_sha256(), NULL, publicKeyObject) <= 0)
		return false;
	
	if (EVP_DigestVerifyUpdate(verifyingContext, &data[0], data.size()) <= 0)
		return false;
	
	int AuthStatus = EVP_DigestVerifyFinal(verifyingContext, &signature[0], signature.size());
	if (AuthStatus == 1)
	{
		EVP_MD_CTX_destroy(verifyingContext);
		return true;
	}

	EVP_MD_CTX_destroy(verifyingContext);
	return false;
}

RSA *RSAVerifier::getRsaObjectFromPublicKey(std::string publicKey)
{
	RSA *rsa = NULL;
	BIO *bio = BIO_new_mem_buf(publicKey.c_str(), publicKey.size());

	if (bio == NULL)
		return NULL;

	rsa = PEM_read_bio_RSA_PUBKEY(bio, &rsa, NULL, NULL);
	return rsa;
}
