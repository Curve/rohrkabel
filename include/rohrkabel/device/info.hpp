#pragma once

#include "../spa/dict.hpp"
#include "../spa/param.hpp"

#include <vector>
#include <cstdint>

struct pw_device_info;

namespace pipewire
{
    struct device_info
    {
        spa::dict props;
        std::uint32_t id;
        std::uint64_t change_mask;
        std::vector<spa::param_info> params;

      public:
        static device_info from(const pw_device_info *);
    };
} // namespace pipewire
