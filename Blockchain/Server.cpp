#include "Server.hpp"
#include "Messages.hpp"

namespace bc {

Server::Server(boost::asio::io_service& ios, short port)
    : ios(ios)
    , acceptor(ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{}

void Server::run() {
    std::shared_ptr<Session> session = std::make_shared<Session>(numSessions, ios, this);
    ++numSessions;
    acceptor.async_accept(session->getSocket(), boost::bind(&Server::onAccept, this,
        session,
        boost::asio::placeholders::error));
}

void Server::onAccept(std::shared_ptr<Session> session, boost::system::error_code const& err) {
    if (!err) {
        std::cout << "User (sessionId=" << session->getId() << ") connected" << std::endl;
        session->startAsync();
        session = std::make_shared<Session>(numSessions, ios, this);
        ++numSessions;
        acceptor.async_accept(session->getSocket(), boost::bind(&Server::onAccept, this,
            session,
            boost::asio::placeholders::error));
    } else {
        std::cerr << "Error: " + err.message() << std::endl;
        session.reset();
    }
}

void Server::receive(Session* session, uint8_t const* data, size_t size) {
    size_t remainedSize = size;
    uint8_t const* dataPtr = data;

    while (remainedSize) {
        Message const& msg = *reinterpret_cast<Message const*>(dataPtr);

        if (msg.type == MessageType::userRegister) {
            std::cout << "UserRegister" << std::endl;
            users.push_back(session);
        } else if (msg.type == MessageType::minerRegister) {
            std::cout << "MinerRegister" << std::endl;
            miners.push_back(session);
        } else if (msg.type == MessageType::blockReq) {
            std::cout << "Block request" << std::endl;
            for (Session* minerSession : miners) {
                minerSession->send(data, size);
            }
        } else if (msg.type == MessageType::blockRsp) {
            std::cout << "Block response" << std::endl;
            for (Session* userSession : users) {
                userSession->send(data, size);
            }
        }

        remainedSize -= (sizeof(Message) + msg.size);
        dataPtr += (sizeof(Message) + msg.size);
    }
}

} // namespace bc
