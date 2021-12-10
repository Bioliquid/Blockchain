#pragma once

#include "Session.hpp"
#include "IReceiver.hpp"

#include <vector>

namespace bc {

class Server : public IReceiver {
public:
    Server(boost::asio::io_service&, short);
    virtual ~Server() = default;

    void run();
    void onAccept(std::shared_ptr<Session>, boost::system::error_code const&);

    virtual void receive(Session*, uint8_t const*, size_t) override;

private:
    uint64_t                        numSessions = 0;
    boost::asio::io_service&        ios;
    boost::asio::ip::tcp::acceptor  acceptor;

    std::vector<Session*>           users;
    std::vector<Session*>           miners;
};

} // namespace bc
