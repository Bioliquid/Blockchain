#include "Client.hpp"

namespace bc {

Client::Client(boost::asio::io_service& ios, short port)
    : ios(ios)
    , ep(boost::asio::ip::address::from_string("127.0.0.1"), port)
{}

void Client::run() {
    session = std::make_shared<Session>(0, ios, this);
    session->getSocket().async_connect(ep,
                                        boost::bind(&Client::onConnectBase, this,
                                        session,
                                        boost::asio::placeholders::error));
}

void Client::onConnectBase(std::shared_ptr<Session> connectedSession, boost::system::error_code const& err) {
    if (session != connectedSession) {
        std::cerr << "Connected session differs from stored one" << std::endl;
        return;
    }

    if (!err) {
        std::cout << "Connection (sessionId=0) is established" << std::endl;
        onConnect();
    } else {
        setlocale(0, "");
        std::cerr << "err: " + err.message() << std::endl;
        session.reset();
    }
}

} // namespace bc
