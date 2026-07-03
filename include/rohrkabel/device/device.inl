#pragma once

#include "device.hpp"

namespace pipewire
{
    template <detail::listener<device::raw_type> Listener>
    Listener device::listen() const
    {
        return {get()};
    }
} // namespace pipewire
