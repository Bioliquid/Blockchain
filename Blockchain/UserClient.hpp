#pragma once

#include "Client.hpp"
#include "BlockChain.hpp"
#include "RsaKeyPair.hpp"

namespace bc {

class UserClient : public Client {
public:
    UserClient(boost::asio::io_service&, short);

    virtual void receive(Session*, uint8_t const*, size_t) override;

    void submitBlockMessage(Block::Message const&);

private:
    virtual void onConnect() override;

    void sendRegistrationRequest();
    void sendNextBlockReq();

    RsaKeyPair                      keys;
    BlockChain                      blockchain;
    uint64_t                        blockId = 0;
    std::vector<Block::Message>     messages;
};

} // namespace bc
