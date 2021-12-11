#pragma once

#include "Block.hpp"

#include <vector>

namespace bc {

class BlockChain {
public:
    BlockChain();

    void addBlock(Block const& block) { blockchain.push_back(block); }

    uint64_t getLastBlockUid() const { return blockchain.back().uid; }
    Hash const& getLastBlockHash() const { return blockchain.back().hash; }

private:
    std::vector<Block> blockchain;
};

} // namespace bc
