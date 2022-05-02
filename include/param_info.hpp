#pragma once
#include <cstdint>

namespace pipewire
{
    struct param_info
    {
        std::uint32_t id;
        std::uint32_t user;
        std::uint32_t flags;
    };
} // namespace pipewire