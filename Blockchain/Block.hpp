#pragma once

#include "Hash.hpp"

#include <vector>
#include <string>

namespace bc {

class BlockChain;

struct Block {
    using Message = std::string;

    Block() = default;

    Block(uint64_t inUid, Hash const& inPrevHash, Hash const& inHash, uint64_t inNonce, Message const& inData)
        : uid(inUid)
        , previousHash(inPrevHash)
        , hash(inHash)
        , nonce(inNonce)
        , data(inData)
    {}

    uint64_t    uid;
    Hash        previousHash;
    Hash        hash;
    uint64_t    nonce;
    Message     data;
};

} // namespace bc
