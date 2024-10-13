#pragma once

#include "metadata.hpp"

namespace pipewire
{
    template <class Listener>
        requires detail::valid_listener<Listener, metadata::raw_type>
    Listener metadata::listen()
    {
        return {get()};
    }
} // namespace pipewire
