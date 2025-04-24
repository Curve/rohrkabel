#pragma once

#include "../spa/dict.hpp"

#include <string>
#include <cstdint>

struct pw_core_info;

namespace pipewire
{
    struct core_info
    {
        spa::dict props;

      public:
        std::uint32_t id;
        std::uint32_t cookie;

      public:
        std::string name;
        std::string version;
        std::string user_name;
        std::string host_name;

      public:
        static core_info from(const pw_core_info *);
    };
} // namespace pipewire
