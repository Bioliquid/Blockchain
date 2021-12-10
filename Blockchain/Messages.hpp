#pragma once

#include <cstdint>

namespace bc {

enum class MessageType : uint8_t {
      userRegister
    , minerRegister
    , blockReq
    , blockRsp
};

struct Message {
    MessageType type;
    uint64_t    size = 0;
    uint8_t     buffer[];
};

} // namespace bc
