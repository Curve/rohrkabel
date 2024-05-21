#pragma once

#include "device.hpp"

namespace pipewire
{
    template <class Listener>
        requires valid_listener<Listener, device::raw_type>
    Listener device::listen()
    {
        return {get()};
    }
} // namespace pipewire
