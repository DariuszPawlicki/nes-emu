#pragma once

#include <bitset>

namespace chips_commons {
    template<std::size_t Bits>
    using Register = std::bitset<Bits>;
}
