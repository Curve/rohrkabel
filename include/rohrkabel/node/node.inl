#pragma once

#include "node.hpp"

namespace pipewire
{
    template <detail::listener<node::raw_type> Listener>
    Listener node::listen() const
    {
        return {get()};
    }
} // namespace pipewire
