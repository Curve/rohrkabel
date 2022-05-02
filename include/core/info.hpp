#pragma once
#include "dict.hpp"

#include <string>
#include <cstdint>

namespace pipewire
{
    struct core_info
    {
        dict props;

        std::uint32_t id;
        std::uint32_t cookie;

        std::string name;
        std::string version;
        std::string user_name;
        std::string host_name;
    };
} // namespace pipewire