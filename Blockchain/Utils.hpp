#pragma once

#include "Hash.hpp"

#include <cstring>
#include <tuple>

namespace bc {

template<typename T>
void memzero(T& t) {
    std::memset(&t, 0, sizeof(T));
}

std::pair<uint64_t, Hash> findHash(uint64_t, Hash, Hash);

} // namespace bc
