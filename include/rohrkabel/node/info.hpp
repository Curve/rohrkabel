#pragma once
#include "../spa/dict.hpp"
#include "../spa/param.hpp"

#include <string>
#include <vector>
#include <cstdint>

namespace pipewire
{
    enum class node_state
    {
        error = -1,
        suspended,
        idle,
        running
    };

    struct node_info
    {
        std::uint32_t id;

        std::uint32_t max_input_ports;
        std::uint32_t max_output_ports;

        std::uint32_t n_input_ports;
        std::uint32_t n_output_ports;

        spa::dict props;
        node_state state;

        std::string error;
        std::uint64_t change_mask;

        std::vector<param_info> params;
    };
} // namespace pipewire