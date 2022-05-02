#pragma once
#include "dict.hpp"

#include <string>
#include <cstdint>

namespace pipewire
{
    struct global
    {
        std::uint32_t id;
        std::uint32_t version;
        std::uint32_t permissions;

        dict props;
        std::string type;
    };
} // namespace pipewire