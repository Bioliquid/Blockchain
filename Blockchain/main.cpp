#include "Hash.hpp"
#include "merklecpp.h"
#include "RsaKeyPair.hpp"
#include "Signature.hpp"

#include "Server.hpp"
#include "UserClient.hpp"
#include "MinerClient.hpp"

#include <iostream>
#include <boost/asio.hpp>

int main() {
    std::cout << "Choose type of application (0 - server, 1 - user, 2 - miner): ";
    int choice;
    std::cin >> choice;

    bc::Hash hash("hello world");
    
    std::cout << hash.toString() << std::endl;

    merkle::Tree::Hash mHash(hash);
    std::cout << mHash.to_string() << std::endl;

    boost::asio::io_service service;
    constexpr short port = 8001;

    bc::RsaKeyPair keys;
    if (keys.generate() == false) {
        std::cout << "failed to generate" << std::endl;
    }

    char const* message = "hello";

    bc::Signature sign;
    if (sign.sign(keys.getPrivateKey(), keys.getPrivateKeyLen(), message, strlen(message)) == false) {
        std::cout << "failed to sign" << std::endl;
    }

    bc::B64Message b64msg;
    b64msg.encode(sign);

    bc::Signature outSign;
    b64msg.decode(outSign);

    bool verified = false;
    if (outSign.verify(keys.getPublicKey(), keys.getPublicKeyLen(), message, strlen(message), verified) == false) {
        std::cout << "failed to verify" << std::endl;
    }

    std::cout << "Is verified " << verified << std::endl;

    if (choice == 0) {
        bc::Server server(service, port);
        server.run();
        service.run();
    } else if (choice == 1) {
        bc::UserClient client(service, port);

        bc::Block::Messages firstTransaction = {"transaction #1"};
        bc::Block::Messages secondTransaction = {"transaction #2.1", "transaction #2.2"};
        client.submitBlockMessage(firstTransaction);
        client.submitBlockMessage(secondTransaction);

        client.run();
        service.run();
    } else if (choice == 2) {
        bc::MinerClient client(service, port);
        client.run();
        service.run();
    }
}
