#pragma once
#include "spa/dict.hpp"
#include "param_info.hpp"

#include <string>
#include <vector>
#include <cstdint>

namespace pipewire
{
    struct device_info
    {
        spa::dict props;
        std::uint32_t id;
        std::uint64_t change_mask;
        std::vector<param_info> params;
    };
} // namespace pipewire