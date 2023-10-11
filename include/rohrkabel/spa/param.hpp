#pragma once
#include <cstdint>

namespace pipewire
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
    };
} // namespace pipewire
