#pragma once

#include <vector>
#include <cstdint>

struct spa_param_info;

namespace pipewire::spa
{
    enum class param_flags : std::uint8_t
    {
        read       = 2,
        write      = 4,
        read_write = read | write,
    };

    struct param_info
    {
        std::uint32_t id;
        param_flags flags;

      public:
        static param_info from(spa_param_info);
        static std::vector<param_info> from(std::uint32_t, spa_param_info *);
    };
} // namespace pipewire::spa
