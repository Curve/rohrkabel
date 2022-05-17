#pragma once
#include "spa/dict.hpp"

#include <string>
#include <cstdint>

namespace pipewire
{
    struct global
    {
        std::uint32_t id;
        std::uint32_t version;
        std::uint32_t permissions;

        spa::dict props;
        std::string type;
    };
} // namespace pipewire