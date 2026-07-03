#pragma once

#include "link.hpp"

namespace pipewire
{
    template <detail::listener<link::raw_type> Listener>
    Listener link::listen() const
    {
        return {get()};
    }
} // namespace pipewire
