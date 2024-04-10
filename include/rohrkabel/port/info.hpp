#pragma once

#include "../spa/dict.hpp"
#include "../spa/param.hpp"

#include <vector>
#include <cstdint>

struct pw_port_info;

namespace pipewire
{
    enum class port_direction : std::uint8_t
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
        std::vector<spa::param_info> params;

      public:
        static port_info from(const pw_port_info *);
    };
} // namespace pipewire
