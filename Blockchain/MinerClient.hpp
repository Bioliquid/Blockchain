#pragma once

#include "Client.hpp"
#include "Block.hpp"

#include <boost/thread.hpp>

namespace bc {

class MinerClient : public Client {
public:
    MinerClient(boost::asio::io_service&, short);

    virtual void receive(Session*, uint8_t const* data, size_t size) override;

private:
    virtual void onConnect() override;
    void sendBlock(Block const&);

    void sendRegistrationRequest();
};

} // namespace bc
