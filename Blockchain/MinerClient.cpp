#include "MinerClient.hpp"
#include "Messages.hpp"
#include "Signature.hpp"
#include "Utils.hpp"

namespace bc {

MinerClient::MinerClient(boost::asio::io_service& ioService, short port)
    : Client(ioService, port)
{}

void MinerClient::receive(Session*, uint8_t const* data, size_t size) {
    Message const& msg = *reinterpret_cast<Message const*>(data);

    if (msg.type != MessageType::blockReq) {
        std::cerr << "Message type is not block request" << std::endl;
    }

    std::cout << "Got block request" << std::endl;

    BlockReq const& req = *reinterpret_cast<BlockReq const*>(msg.buffer);
    uint8_t const* blockReqData = req.buffer;

    // signature
    size_t b64msgLen = *(size_t*)blockReqData;
    blockReqData += sizeof(size_t);
    
    B64Message b64msg;
    b64msg.alloc((char const*)blockReqData, b64msgLen);
    blockReqData += b64msgLen;
    
    Signature signature;
    b64msg.decode(signature);
    
    // public key
    size_t publicKeyLen = *(size_t*)blockReqData;
    blockReqData += sizeof(size_t);
    char* publicKey = new char[publicKeyLen + 1];
    std::memcpy(publicKey, blockReqData, publicKeyLen);
    publicKey[publicKeyLen] = 0;
    blockReqData += publicKeyLen;
    
    // message
    size_t messageLen = *(size_t*)blockReqData;
    blockReqData += sizeof(size_t);

    bool authentic;
    if (signature.verify(publicKey, publicKeyLen, (char const*)blockReqData, messageLen, authentic) == false) {
        std::cerr << "Failed to verify transaction" << std::endl;
        return;
    }

    if (authentic) {
        std::cout << "Transaction is verified" << std::endl;
    }

    delete[] publicKey;

    std::string message((char const*)blockReqData, messageLen);
    auto [nonce, hash] = findHash(req.uid, req.previousHash, Hash{message});

    Block block{req.uid, req.previousHash, hash, nonce, message};

    sendBlock(block);

    session->start();
}

void MinerClient::onConnect() {
    sendRegistrationRequest();
    session->start();
}

void MinerClient::sendBlock(Block const& block) {
    // send calculated block
    uint8_t* msgData = session->allocateMessage();

    Message& msg = *new (msgData) Message;
    msg.type = MessageType::blockRsp;

    BlockRsp& rsp = *new (msg.buffer) BlockRsp;
    rsp.uid = block.uid;
    rsp.previousHash = block.previousHash;
    rsp.hash = block.hash;
    rsp.nonce = block.nonce;

    uint8_t* blockRspData = rsp.buffer;

    *(size_t*)blockRspData = block.data.size();
    blockRspData += sizeof(size_t);
    std::memcpy(blockRspData, block.data.c_str(), block.data.size());
    blockRspData += block.data.size();

    msg.size = blockRspData - msgData - sizeof(Message);

    session->send(msgData, blockRspData - msgData);
}

void MinerClient::sendRegistrationRequest() {
    uint8_t* msgData = session->allocateMessage();

    Message& msg = *new (msgData) Message;
    msg.type = MessageType::minerRegister;
    msg.size = 0;
    session->send(msgData, sizeof(msg));
}

} // namespace bc
