#include "UserClient.hpp"
#include "Messages.hpp"

#include <boost/thread.hpp>

namespace bc {

UserClient::UserClient(boost::asio::io_service& ioService, short port)
    : Client(ioService, port)
{}

void UserClient::receive(Session*, uint8_t const* data, size_t size) {
    std::cout << "recv: " << data << std::endl;
}

void UserClient::onConnect() {
    sendRegistrationRequest();
    sendNextBlockReq();

    session->start();
}

void UserClient::submitBlockMessage(Block::Messages const& msg) {
    messages.push_back(msg);
}

void UserClient::sendRegistrationRequest() {
    uint8_t* msgData = session->allocateMessage();

    Message& msg = *new (msgData) Message;
    msg.type = MessageType::userRegister;
    msg.size = 0;
    session->send(msgData, sizeof(msg));
}

void UserClient::sendNextBlockReq() {
    /*uint8_t* msgData = session->allocateMessage();
    Message& msg = *new (msgData) Message;
    msg.type = MessageType::blockReq;
    msg.size = strlen(text);
    std::memcpy(msg.buffer, text, sizeof(text) + msg.size);
    session->send(msgData, sizeof(msg) + msg.size);*/

    ++blockId;
}

} // namespace bc
