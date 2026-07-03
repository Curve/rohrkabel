#pragma once

#include "client.hpp"

namespace pipewire
{
    template <detail::listener<client::raw_type> Listener>
    Listener client::listen() const
    {
        return {get()};
    }
} // namespace pipewire
