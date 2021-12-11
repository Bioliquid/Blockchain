#include "Server.hpp"
#include "UserClient.hpp"
#include "MinerClient.hpp"

int main() {
    std::cout << "Choose type of application (0 - server, 1 - user, 2 - miner): ";
    int choice;
    std::cin >> choice;

    boost::asio::io_service service;
    constexpr short port = 8001;

    if (choice == 0) {
        bc::Server server(service, port);
        server.run();
        service.run();
    } else if (choice == 1) {
        bc::UserClient client(service, port);

        bc::Block::Message firstTransaction = "transaction #1";
        bc::Block::Message secondTransaction = "transaction #2";
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
