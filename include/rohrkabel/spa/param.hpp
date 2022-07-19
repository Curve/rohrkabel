#pragma once
#include <cstdint>

namespace pipewire
{
    enum class param_info_flags
    {
        read = 2,
        write = 4,
        read_write = read | write,
    };

    struct param_info
    {
        std::uint32_t id;
        param_info_flags flags;
    };
} // namespace pipewire