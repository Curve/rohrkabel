#pragma once

#include "node.hpp"

namespace pipewire
{
    template <class Listener>
        requires valid_listener<Listener, node::raw_type>
    Listener node::listen()
    {
        return {get()};
    }
} // namespace pipewire
