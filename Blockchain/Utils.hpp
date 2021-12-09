#pragma once

#include <cstring>

namespace bc {

template<typename T>
void memzero(T& t) {
    std::memset(&t, 0, sizeof(T));
}

} // namespace bc
