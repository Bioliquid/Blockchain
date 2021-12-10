#pragma once

#include "Client.hpp"

namespace bc {

class UserClient : public Client {
public:
    UserClient(boost::asio::io_service&, short);

    virtual void receive(Session*, uint8_t const*, size_t) override;

private:
    virtual void onConnect() override;

    void sendRegistrationRequest();
};

} // namespace bc
