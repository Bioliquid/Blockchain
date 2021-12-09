#include "Hash.hpp"
#include "merklecpp.h"

#include <iostream>
#include <boost/asio.hpp>

int main() {
    bc::Hash hash("hello world");
    
    std::cout << hash.toString() << std::endl;

    merkle::Tree::Hash mHash(hash);
    std::cout << mHash.to_string() << std::endl;
}
