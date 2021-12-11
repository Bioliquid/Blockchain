#include "Session.hpp"

namespace bc {

Session::Session(uint64_t sid, boost::asio::io_service& ioService, IReceiver* inReceiver)
    : sessionId(sid)
    , receiver(inReceiver)
    , socket(ioService)
{}

void Session::start() {
    size_t bytesRead = socket.read_some(boost::asio::buffer(recvData, maxBufferSize));
    std::cout << "Received " << bytesRead << " bytes" << std::endl;
    receiver->receive(this, recvData, bytesRead);
}

void Session::startAsync() {
    socket.async_read_some(
        boost::asio::buffer(recvData, maxBufferSize),
        boost::bind(&Session::handleRead, this,
        shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void Session::send(uint8_t const* data, size_t size) {
    boost::system::error_code errorCode;
    size_t bytesSent = socket.write_some(boost::asio::buffer(data, size), errorCode);
    std::cout << "Sent " << bytesSent << " bytes" << std::endl;
    if (errorCode != boost::system::errc::success) {
        std::cerr << "Cant send message" << std::endl;
    }
}

void Session::handleRead(std::shared_ptr<Session>&, boost::system::error_code const& err, size_t bytesRead) {
    std::cout << "Received " << bytesRead << " bytes" << std::endl;
    if (!err) {
        receiver->receive(this, recvData, bytesRead);
        startAsync();
    } else if (err == boost::asio::error::eof || err == boost::asio::error::connection_reset) {
        std::cout << "User (sessionId=" << sessionId << ") disconnected" << std::endl;
    } else {
        std::cerr << "Error (recv): " << err.message() << std::endl;
    }
}

} // namespace bc
