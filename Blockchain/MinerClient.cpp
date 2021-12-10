#include "MinerClient.hpp"
#include "Messages.hpp"

namespace bc {

MinerClient::MinerClient(boost::asio::io_service& ioService, short port)
    : Client(ioService, port)
{}

void MinerClient::receive(Session*, uint8_t const* data, size_t size) {
    Message const& msg = *reinterpret_cast<Message const*>(data);

    if (msg.type != MessageType::blockReq) {
        std::cerr << "Message type is not block request" << std::endl;
    }

    std::string str((char const*)msg.buffer, msg.size);

    std::cout << "recv: " << str << std::endl;
    session->start();
}

void MinerClient::onConnect() {
    sendRegistrationRequest();
    session->start();
}

void MinerClient::sendRegistrationRequest() {
    uint8_t* msgData = session->allocateMessage();

    Message& msg = *new (msgData) Message;
    msg.type = MessageType::minerRegister;
    msg.size = 0;
    session->send(msgData, sizeof(msg));
}

} // namespace bc
