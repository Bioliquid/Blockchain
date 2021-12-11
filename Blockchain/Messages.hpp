#pragma once

#include "Hash.hpp"

namespace bc {

enum class MessageType : uint8_t {
      userRegister
    , minerRegister
    , blockReq
    , blockRsp
};

struct Message {
    MessageType type;
    uint64_t    size = 0;
    uint8_t     buffer[];
};

struct BlockReq {
    uint64_t        uid;
    Hash            previousHash;
    // [len: 8 bytes, data: len bytes]
    // 0 idx: signature
    // 1 idx: public key
    // 2 idx: message
    uint8_t         buffer[];
};

struct BlockRsp {
    uint64_t    uid;
    Hash        previousHash;
    Hash        hash;
    uint64_t    nonce;
    // [len: 8 bytes, data: len bytes]
    // 0 idx: message
    uint8_t         buffer[];
};

} // namespace bc
