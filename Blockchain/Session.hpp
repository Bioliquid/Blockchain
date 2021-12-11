#pragma once

#include "IReceiver.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <memory>
#include <cstdint>

namespace bc {

class Session : public std::enable_shared_from_this<Session> {
    static constexpr uint64_t maxBufferSize = 4096;

public:
    Session(uint64_t, boost::asio::io_service&, IReceiver*);

    void start();
    void startAsync();

    void send(uint8_t const*, size_t);

    uint8_t* allocateMessage() { return sendData; }

    inline uint64_t getId() const { return sessionId; }
    inline boost::asio::ip::tcp::socket& getSocket() { return socket; }

private:
    void handleRead(std::shared_ptr<Session>&, boost::system::error_code const&, size_t);

    uint64_t                        sessionId;
    IReceiver*                      receiver;
    boost::asio::ip::tcp::socket    socket;
    uint8_t                         recvData[maxBufferSize] = {};
    uint8_t                         sendData[maxBufferSize] = {};
};

} // namespace bc
