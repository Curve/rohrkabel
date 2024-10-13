#pragma once

#include "proxy.hpp"

namespace pipewire
{
    template <class Listener>
        requires detail::valid_listener<Listener, proxy::raw_type>
    Listener proxy::listen()
    {
        return {get()};
    }
} // namespace pipewire
