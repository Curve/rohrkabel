#pragma once
#include "dict.hpp"
#include "param_info.hpp"

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

        dict props;
        std::vector<param_info> params;
    };
} // namespace pipewire