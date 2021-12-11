#pragma once

#define HAVE_OPENSSL
#include "merklecpp.h"

#include <openssl/sha.h>
#include <cstring>
#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

namespace bc {

template<size_t size>
class HashT {
    using Type = char[size + 1];

public:
    HashT() {
        std::memset(hash, '0', size + 1);
    }

    explicit HashT(std::string const& str) {
        uint8_t inHash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, str.c_str(), str.size());
        SHA256_Final(inHash, &sha256);

        char* outputBuffer = hash;
        for (uint8_t i = 0; i < SHA256_DIGEST_LENGTH; ++i, outputBuffer += 2) {
            sprintf(outputBuffer, "%02x", inHash[i]);
        }

        hash[size] = 0;
    }

    explicit HashT(std::vector<std::string> const& data) {
        std::vector<merkle::Tree::Hash> hashes;
        std::transform(data.begin(), data.end(), hashes.begin(), [](std::string const& str) {
            return merkle::Tree::Hash(HashT(str));
        });

        merkle::Tree tree;
        tree.insert(hashes);
        merkle::Tree::Hash root = tree.root();

        char* outputBuffer = hash;
        for (uint8_t i = 0; i < SHA256_DIGEST_LENGTH; ++i, outputBuffer += 2) {
            sprintf(outputBuffer, "%02x", root.bytes[i]);
        }
        hash[size] = 0;
    }

    HashT& operator=(HashT const& rhs) {
        std::memcpy(hash, rhs.hash, size);
        hash[size] = 0;

        return *this;
    }

    void compress(HashT const& rhs) {
        merkle::Tree::Hash left(toString());
        merkle::Tree::Hash right(rhs.toString());
        merkle::Tree::Hash out;
        merkle::sha256_compress_openssl(left, right, out);
        char* outputBuffer = hash;
        for (uint8_t i = 0; i < SHA256_DIGEST_LENGTH; ++i, outputBuffer += 2) {
            sprintf(outputBuffer, "%02x", out.bytes[i]);
        }
        hash[size] = 0;
    }

    HashT& operator+=(HashT const& rhs) {
        compress(rhs);
        return *this;
    }

    friend HashT operator+(HashT lhs, HashT const& rhs) {
        lhs += rhs;
        return lhs;
    }

    bool verifyDifficulty() {
        constexpr uint64_t difficulty = 2;
        for (uint64_t i = 0; i < difficulty; ++i) {
            if (hash[i] != '0') {
                return false;
            }
        }
        return true;
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
