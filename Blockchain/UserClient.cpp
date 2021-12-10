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
    
    char const* text = "hello miner";
    
    uint8_t* msgData = session->allocateMessage();
    Message& msg = *new (msgData) Message;
    msg.type = MessageType::blockReq;
    msg.size = strlen(text);
    std::memcpy(msg.buffer, text, sizeof(text) + msg.size);
    session->send(msgData, sizeof(msg) + msg.size);
    
    session->start();



    /*static int i = 0;
    while (i < 5) {
        uint8_t buffer[] = "helloxx";
        buffer[5] = '0' + (i / 10) % 10;
        buffer[6] = '0' + (i % 10);
        boost::system::error_code errorCode;
        session->getSocket().write_some(boost::asio::buffer(buffer, sizeof(buffer)), errorCode);
        if (errorCode != boost::system::errc::success) {
            std::cerr << "cant write_some" << std::endl;
        }
        boost::this_thread::sleep(boost::posix_time::millisec(1000));
        ++i;
    }
    session->start();*/
}

void UserClient::sendRegistrationRequest() {
    uint8_t* msgData = session->allocateMessage();

    Message& msg = *new (msgData) Message;
    msg.type = MessageType::userRegister;
    msg.size = 0;
    session->send(msgData, sizeof(msg));
}

} // namespace bc
