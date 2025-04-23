#pragma once

#include "device.hpp"

namespace pipewire
{
    template <detail::Listener<device::raw_type> Listener>
    Listener device::listen() const
    {
        return {get()};
    }
} // namespace pipewire
