#pragma once

#include "Session.hpp"
#include "IReceiver.hpp"

namespace bc {

class Client : public IReceiver {
public:
    Client(boost::asio::io_service&, short);
    virtual ~Client() = default;

    void run();

private:
    void onConnectBase(std::shared_ptr<Session>, boost::system::error_code const&);
    virtual void onConnect() = 0;

    boost::asio::io_service&        ios;
    boost::asio::ip::tcp::endpoint  ep;

protected:
    std::shared_ptr<Session>        session;
};

} // namespace bc
