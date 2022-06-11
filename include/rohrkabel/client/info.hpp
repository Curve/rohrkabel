#pragma once
#include "../spa/dict.hpp"

#include <cstdint>

namespace pipewire
{
    struct client_info
    {
        spa::dict props;
        std::uint32_t id;
        std::uint64_t change_mask;
    };
} // namespace pipewire