#include "Hash.hpp"
#include "merklecpp.h"

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

    if (choice == 0) {
        bc::Server server(service, port);
        server.run();
        service.run();
    } else if (choice == 1) {
        bc::UserClient client(service, port);
        client.run();
        service.run();
    } else if (choice == 2) {
        bc::MinerClient client(service, port);
        client.run();
        service.run();
    }
}
