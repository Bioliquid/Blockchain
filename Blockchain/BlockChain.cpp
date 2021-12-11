#include "BlockChain.hpp"
#include "Utils.hpp"

#include <iostream>

namespace bc {

BlockChain::BlockChain() {
    Block block;
    block.uid = 0;
    block.previousHash = Hash{};
    block.data = "Genesis block";

    Hash dataHash({block.data});

    auto [nonce, hash] = findHash(block.uid, block.previousHash, Hash{block.data});
    block.nonce = nonce;
    block.hash = hash;

    blockchain.emplace_back(block);
}

} // namespace bc
