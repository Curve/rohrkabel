#pragma once

#include "../spa/dict.hpp"

#include <cstdint>

struct pw_link_info;

namespace pipewire
{
    enum class link_state : std::int8_t
    {
        error = -2,
        unlinked,
        init,
        negotiating,
        allocating,
        paused,
        active
    };

    struct link_info
    {
        spa::dict props;
        link_state state;

        std::uint32_t id;
        std::uint64_t change_mask;

        struct
        {
            std::uint32_t port;
            std::uint32_t node;
        } input;

        struct
        {
            std::uint32_t port;
            std::uint32_t node;
        } output;

      public:
        static link_info from(const pw_link_info *);
    };
} // namespace pipewire
