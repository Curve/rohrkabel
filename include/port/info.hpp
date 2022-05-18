#pragma once
#include "spa/dict.hpp"

#include <cstdint>

namespace pipewire
{
    enum class port_direction
    {
        input,
        output
    };

    struct port_info
    {
        std::uint32_t id;
        port_direction direction;
        std::uint64_t change_mask;

        spa::dict props;
    };
} // namespace pipewire