#pragma once

#include "client.hpp"

namespace pipewire
{
    template <class Listener>
        requires detail::valid_listener<Listener, client::raw_type>
    Listener client::listen()
    {
        return {get()};
    }
} // namespace pipewire
