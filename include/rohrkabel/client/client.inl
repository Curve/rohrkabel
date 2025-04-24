#pragma once

#include "client.hpp"

namespace pipewire
{
    template <detail::Listener<client::raw_type> Listener>
    Listener client::listen() const
    {
        return {get()};
    }
} // namespace pipewire
