#pragma once

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <cstdio>

namespace bc {

class RsaKeyPair {
public:
    bool generate() {
        int result = 0;

        // 1. generate rsa key
        BIGNUM* bne = BN_new();
        result = BN_set_word(bne, RSA_F4);
        if (result != 1) {
            BN_free(bne);
            return false;
        }

        RSA* r = RSA_new();
        result = RSA_generate_key_ex(r, 2048, bne, nullptr);
        if (result != 1) {
            RSA_free(r);
            BN_free(bne);
            return false;
        }

        // 2. save public key
        BIO* bp_public = BIO_new(BIO_s_mem());
        result = PEM_write_bio_RSAPublicKey(bp_public, r);
        if (result != 1) {
            BIO_free_all(bp_public);
            RSA_free(r);
            BN_free(bne);
            return false;
        }

        // 3. save private key
        BIO* bp_private = BIO_new(BIO_s_mem());
        result = PEM_write_bio_RSAPrivateKey(bp_private, r, nullptr, nullptr, 0, nullptr, nullptr);

        //4. Get the keys are PEM formatted strings
        privateKeyLen = BIO_pending(bp_private);
        publicKeyLen = BIO_pending(bp_public);

        privateKey = (char*)malloc(privateKeyLen + 1);
        publicKey = (char*)malloc(publicKeyLen + 1);

        BIO_read(bp_private, privateKey, privateKeyLen);
        BIO_read(bp_public, publicKey, publicKeyLen);

        privateKey[privateKeyLen] = '\0';
        publicKey[publicKeyLen] = '\0';

        // printf("\n%s\n%s\n", privateKey, publicKey);

        // 4. free
        BIO_free_all(bp_public);
        BIO_free_all(bp_private);
        RSA_free(r);
        BN_free(bne);

        if (!verify()) {
            return false;
        }

        return result == 1;
    }

    ~RsaKeyPair() {
        free(privateKey);
        free(publicKey);
    }

    inline char const* getPrivateKey() const { return privateKey; }
    inline size_t getPrivateKeyLen() const { return privateKeyLen; }
    inline char const* getPublicKey() const { return publicKey; }
    inline size_t getPublicKeyLen() const { return publicKeyLen; }

private:
    bool verify() {
        // verify if you are able to re-construct the keys
        BIO* pbkeybio = BIO_new_mem_buf((void*)publicKey, publicKeyLen);
        if (pbkeybio == nullptr) {
            return false;
        }
        publicRsa = PEM_read_bio_RSAPublicKey(pbkeybio, &publicRsa, nullptr, nullptr);
        if (publicRsa == nullptr) {
            char buffer[120];
            ERR_error_string(ERR_get_error(), buffer);
            printf("Error reading public key:%s\n", buffer);
        }
        evpPublicKey = EVP_PKEY_new();
        EVP_PKEY_assign_RSA(evpPublicKey, publicRsa);

        BIO* pkeybio = BIO_new_mem_buf((void*)privateKey, privateKeyLen);
        if (pkeybio == nullptr) {
            return false;
        }
        privateRsa = PEM_read_bio_RSAPrivateKey(pkeybio, &privateRsa, nullptr, nullptr);
        if (privateRsa == nullptr) {
            char buffer[120];
            ERR_error_string(ERR_get_error(), buffer);
            printf("Error reading private key:%s\n", buffer);
        }
        evpPrivateKey = EVP_PKEY_new();
        EVP_PKEY_assign_RSA(evpPrivateKey, privateRsa);

        BIO_free(pbkeybio);
        BIO_free(pkeybio);
        return true;
    }

    RSA* publicRsa = nullptr;
    EVP_PKEY* evpPublicKey = nullptr;
    char* publicKey;
    size_t publicKeyLen;
    
    RSA* privateRsa = nullptr;
    EVP_PKEY* evpPrivateKey = nullptr;
    char* privateKey;
    size_t privateKeyLen;
};

} // namespace bc
