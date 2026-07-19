#pragma once

#include "../spa/dict.hpp"

#include <string>
#include <cstdint>

struct pw_module_info;

namespace pipewire
{
    struct module_info
    {
        std::uint32_t id;

      public:
        std::string args;
        std::string filename;

      public:
        spa::dict props;
        std::string name;

      public:
        std::uint64_t change_mask;

      public:
        static module_info from(const pw_module_info *);
    };
} // namespace pipewire
