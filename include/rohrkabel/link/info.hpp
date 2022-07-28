#pragma once
#include "../spa/dict.hpp"

#include <string>
#include <cstdint>

namespace pipewire
{
    enum class link_state
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
        std::uint32_t id;

        std::uint32_t input_port_id;
        std::uint32_t input_node_id;
        std::uint32_t output_port_id;
        std::uint32_t output_node_id;

        std::uint64_t change_mask;

        spa::dict props;
        link_state state;
    };
} // namespace pipewire