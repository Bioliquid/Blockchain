#pragma once

#include <openssl/sha.h>
#include <cstring>
#include <cstdio>
#include <cstdint>
#include <string>

namespace bc {

template<size_t size>
class HashT {
    using Type = char[size + 1];

public:
    explicit HashT(char const* str) {
        uint8_t inHash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, str, strlen(str));
        SHA256_Final(inHash, &sha256);

        char* outputBuffer = hash;
        for (uint8_t i = 0; i < SHA256_DIGEST_LENGTH; ++i, outputBuffer += 2) {
            sprintf(outputBuffer, "%02x", inHash[i]);
        }

        hash[size + 1] = 0;
    }

    std::string toString() const {
        return hash;
    }

    operator std::string() const {
        return hash;
    }

private:
    Type hash;
};

using Hash = HashT<SHA256_DIGEST_LENGTH * 2>;

} // namespace bc
