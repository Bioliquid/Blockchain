#pragma once

#include <cstdint>

namespace bc {

class Session;

struct IReceiver {
    virtual void receive(Session*, uint8_t const*, size_t) = 0;
};

} // namespace bc
