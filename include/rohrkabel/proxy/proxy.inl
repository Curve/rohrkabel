#pragma once

#include "proxy.hpp"

namespace pipewire
{
    template <detail::Listener<proxy::raw_type> Listener>
    Listener proxy::listen() const
    {
        return {get()};
    }
} // namespace pipewire
