#pragma once

#include "Hash.hpp"

#include <vector>
#include <string>

namespace bc {

class Block {
public:
    using Message = std::string;
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
