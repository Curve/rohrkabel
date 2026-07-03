#pragma once

#include "port.hpp"

namespace pipewire
{
    template <detail::listener<port::raw_type> Listener>
    Listener port::listen() const
    {
        return {get()};
    }
} // namespace pipewire
