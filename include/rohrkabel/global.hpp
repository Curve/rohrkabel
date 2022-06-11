#pragma once
#include "spa/dict.hpp"

#include <string>
#include <cstdint>

namespace pipewire
{
    struct global
    {
      public:
        std::uint32_t id;
        std::uint32_t version;
        std::uint32_t permissions;

        spa::dict props;
        std::string type;

      public:
        [[deprecated("Implicit conversion is deprecated, use `.id` instead")]] operator std::uint32_t() const;
    };
} // namespace pipewire