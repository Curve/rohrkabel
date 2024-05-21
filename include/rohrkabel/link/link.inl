#pragma once

#include "link.hpp"

namespace pipewire
{
    template <class Listener>
        requires valid_listener<Listener, link::raw_type>
    Listener link::listen()
    {
        return {get()};
    }
} // namespace pipewire
