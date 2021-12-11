#include "Utils.hpp"

#define HAVE_OPENSSL
#include "merklecpp.h"

namespace bc {

std::pair<uint64_t, Hash> findHash(uint64_t uid, Hash prevHash, Hash dataHash) {
    Hash header(std::to_string(uid));
    header.compress(prevHash);
    header.compress(dataHash);

    for (uint64_t nonce = 0; nonce < 100000; ++nonce) {
        Hash blockHash = header;
        blockHash.compress(Hash(std::to_string(nonce)));
        if (blockHash.verifyDifficulty()) {
            return std::make_pair(nonce, blockHash);
        }
    }

    return std::make_pair(0, Hash{});
}

} // namespace bc
