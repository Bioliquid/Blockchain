#pragma once

#include "Hash.hpp"

#include <vector>
#include <string>

namespace bc {

class Block {
    struct Message {
        std::string msg;
        Hash        hash;
    };

    using Messages = std::vector<Message>;

public:

private:
    uint64_t    uid;
    Hash        previousHash;
    Hash        hash;
    uint64_t    nonce;
    Messages    data;
};

} // namespace bc
