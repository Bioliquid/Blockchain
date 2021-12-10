#pragma once

#include "Client.hpp"
#include "Block.hpp"

namespace bc {

class UserClient : public Client {
public:
    UserClient(boost::asio::io_service&, short);

    virtual void receive(Session*, uint8_t const*, size_t) override;

    void submitBlockMessage(Block::Messages const&);

private:
    virtual void onConnect() override;

    void sendRegistrationRequest();
    void sendNextBlockReq();

    uint64_t                        blockId = 0;
    std::vector<Block::Messages>    messages;
};

} // namespace bc
