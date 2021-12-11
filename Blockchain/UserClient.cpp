#include "UserClient.hpp"
#include "Messages.hpp"
#include "Signature.hpp"

namespace bc {

UserClient::UserClient(boost::asio::io_service& ioService, short port)
    : Client(ioService, port)
{
    keys.generate();
}

void UserClient::receive(Session*, uint8_t const* data, size_t size) {
    Message const& msg = *reinterpret_cast<Message const*>(data);

    if (msg.type != MessageType::blockRsp) {
        std::cerr << "Message type is not block response" << std::endl;
    }

    std::cout << "Got block response" << std::endl;

    BlockRsp const& rsp = *reinterpret_cast<BlockRsp const*>(msg.buffer);
    uint8_t const* blockRspData = rsp.buffer;

    // message
    size_t messageLen = *(size_t*)blockRspData;
    blockRspData += sizeof(size_t);

    std::string message((char const*)blockRspData, messageLen);

    Block block{rsp.uid, rsp.previousHash, rsp.hash, rsp.nonce, message};
    blockchain.addBlock(block);

    sendNextBlockReq();

    session->start();
}

void UserClient::onConnect() {
    sendRegistrationRequest();
    sendNextBlockReq();

    session->start();
}

void UserClient::submitBlockMessage(Block::Message const& msg) {
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
    if (blockId >= messages.size()) {
        return;
    }

    Block::Message message = messages[blockId];
    Signature signature;
    if (signature.sign(keys.getPrivateKey(), keys.getPrivateKeyLen(), message.c_str(), message.size()) == false) {
        std::cerr << "failed to sign" << std::endl;
    }
    B64Message b64msg;
    b64msg.encode(signature);

    uint8_t* msgData = session->allocateMessage();
    
    Message& msg = *new (msgData) Message;
    msg.type = MessageType::blockReq;
        
    BlockReq& req = *new (msg.buffer) BlockReq;
    req.uid = blockchain.getLastBlockUid() + 1;
    req.previousHash = blockchain.getLastBlockHash();
    uint8_t* blockReqData = req.buffer;

    // signature
    *(size_t*)blockReqData = b64msg.getMsgLen();
    blockReqData += sizeof(size_t);
    std::memcpy(blockReqData, b64msg.getMsg(), b64msg.getMsgLen());
    blockReqData += b64msg.getMsgLen();

    // public key
    *(size_t*)blockReqData = keys.getPublicKeyLen();
    blockReqData += sizeof(size_t);
    std::memcpy(blockReqData, keys.getPublicKey(), keys.getPublicKeyLen());
    blockReqData += keys.getPublicKeyLen();

    // message
    *(size_t*)blockReqData = message.size();
    blockReqData += sizeof(size_t);
    std::memcpy(blockReqData, message.c_str(), message.size());
    blockReqData += message.size();

    msg.size = blockReqData - msgData - sizeof(Message);

    session->send(msgData, blockReqData - msgData);

    ++blockId;
}

} // namespace bc
