#pragma once

#include "port.hpp"

namespace pipewire
{
    template <class Listener>
        requires valid_listener<Listener, port::raw_type>
    Listener port::listen()
    {
        return {get()};
    }
} // namespace pipewire
