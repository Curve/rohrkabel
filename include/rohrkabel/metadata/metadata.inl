#pragma once

#include "metadata.hpp"

namespace pipewire
{
    template <detail::Listener<metadata::raw_type> Listener>
    Listener metadata::listen() const
    {
        return {get()};
    }
} // namespace pipewire
