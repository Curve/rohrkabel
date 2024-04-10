#pragma once

#include "../spa/dict.hpp"

#include <cstdint>

struct pw_client_info;

namespace pipewire
{
    struct client_info
    {
        spa::dict props;
        std::uint32_t id;
        std::uint64_t change_mask;

      public:
        static client_info from(const pw_client_info *);
    };
} // namespace pipewire
