#pragma once

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

namespace bc {

class B64Message;

class Signature {
    friend class B64Message;

public:
    bool sign(char const* privateKey, size_t privateKeyLen, char const* msg, size_t msgLen) {
        EVP_MD_CTX* m_RSASignCtx = EVP_MD_CTX_create();

        BIO* pkeybio = BIO_new_mem_buf((void*)privateKey, privateKeyLen);
        if (pkeybio == nullptr) {
            return false;
        }

        RSA* privateRsa = nullptr;
        privateRsa = PEM_read_bio_RSAPrivateKey(pkeybio, &privateRsa, nullptr, nullptr);
        if (privateRsa == nullptr) {
            char buffer[120];
            ERR_error_string(ERR_get_error(), buffer);
            printf("Error reading private key:%s\n", buffer);
        }

        EVP_PKEY* evpPrivateKey = EVP_PKEY_new();
        EVP_PKEY_assign_RSA(evpPrivateKey, privateRsa);

        if (EVP_DigestSignInit(m_RSASignCtx, nullptr, EVP_sha256(), nullptr, evpPrivateKey) <= 0) {
            return false;
        }

        if (EVP_DigestSignUpdate(m_RSASignCtx, msg, msgLen) <= 0) {
            return false;
        }

        if (EVP_DigestSignFinal(m_RSASignCtx, nullptr, &encMessageLen) <= 0) {
            return false;
        }

        encMessage = (unsigned char*)malloc(encMessageLen);
        if (EVP_DigestSignFinal(m_RSASignCtx, encMessage, &encMessageLen) <= 0) {
            return false;
        }

        BIO_free(pkeybio);
        EVP_MD_CTX_free(m_RSASignCtx);
        return true;
    }

    bool verify(char const* publicKey, size_t publicKeyLen, char const* msg, size_t msgLen, bool& authentic) {
        authentic = false;
        EVP_MD_CTX* m_RSAVerifyCtx = EVP_MD_CTX_create();

        BIO* pbkeybio = BIO_new_mem_buf((void*)publicKey, publicKeyLen);
        if (pbkeybio == nullptr) {
            return false;
        }

        RSA* publicRsa = nullptr;
        publicRsa = PEM_read_bio_RSAPublicKey(pbkeybio, &publicRsa, nullptr, nullptr);
        if (publicRsa == nullptr) {
            char buffer[120];
            ERR_error_string(ERR_get_error(), buffer);
            printf("Error reading public key:%s\n", buffer);
        }

        EVP_PKEY* evpPublicKey = EVP_PKEY_new();
        EVP_PKEY_assign_RSA(evpPublicKey, publicRsa);

        if (EVP_DigestVerifyInit(m_RSAVerifyCtx, NULL, EVP_sha256(), NULL, evpPublicKey) <= 0) {
            return false;
        }
        if (EVP_DigestVerifyUpdate(m_RSAVerifyCtx, msg, msgLen) <= 0) {
            return false;
        }
        int AuthStatus = EVP_DigestVerifyFinal(m_RSAVerifyCtx, encMessage, encMessageLen);
        if (AuthStatus == 1) {
            authentic = true;
            EVP_MD_CTX_free(m_RSAVerifyCtx);
            return true;
        } else if (AuthStatus == 0) {
            authentic = false;
            EVP_MD_CTX_free(m_RSAVerifyCtx);
            return true;
        } else {
            authentic = false;
            EVP_MD_CTX_free(m_RSAVerifyCtx);
            return false;
        }
    }

    ~Signature() {
        free(encMessage);
    }

private:
    unsigned char*  encMessage;
    size_t          encMessageLen;
};

class B64Message {
public:
    void alloc(char const* msg, size_t len) {
        encLen = len;
        encMsg = new char[len];
        std::memcpy(encMsg, msg, len);
    }

    void dealloc() {
        delete[] encMsg;
    }

    void encode(Signature const& signature) {
        BIO* bio, * b64;
        BUF_MEM* bufferPtr;

        b64 = BIO_new(BIO_f_base64());
        bio = BIO_new(BIO_s_mem());
        bio = BIO_push(b64, bio);

        BIO_write(bio, signature.encMessage, signature.encMessageLen);
        BIO_flush(bio);
        BIO_get_mem_ptr(bio, &bufferPtr);
        BIO_set_close(bio, BIO_NOCLOSE);
        BIO_free_all(bio);

        encMsg = (*bufferPtr).data;
        encMsg[(*bufferPtr).length] = 0;
        encLen = (*bufferPtr).length;
    }

    void decode(Signature& signature) {
        BIO* bio, * b64;

        int decodeLen = calcDecodeLength();
        signature.encMessage = (unsigned char*)malloc(decodeLen + 1);
        signature.encMessage[decodeLen] = '\0';

        bio = BIO_new_mem_buf(encMsg, -1);
        b64 = BIO_new(BIO_f_base64());
        bio = BIO_push(b64, bio);

        signature.encMessageLen = BIO_read(bio, signature.encMessage, encLen);
        BIO_free_all(bio);
    }

    size_t getMsgLen() const { return encLen; }
    char const* getMsg() const { return encMsg; }

private:
    size_t calcDecodeLength() {
        size_t padding = 0;

        if (encMsg[encLen - 1] == '=' && encMsg[encLen - 2] == '=') {
            padding = 2;
        } else if (encMsg[encLen - 1] == '=') {
            padding = 1;
        }
        return (encLen * 3) / 4 - padding;
    }

    size_t  encLen;
    char*   encMsg;
};

} // namespace bc
