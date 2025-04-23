#pragma once

#include "node.hpp"

namespace pipewire
{
    template <detail::Listener<node::raw_type> Listener>
    Listener node::listen() const
    {
        return {get()};
    }
} // namespace pipewire
