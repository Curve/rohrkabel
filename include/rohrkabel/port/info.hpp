#pragma once
#include "../spa/dict.hpp"
#include "../spa/param.hpp"

#include <vector>
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
        std::vector<param_info> params;
    };
} // namespace pipewire