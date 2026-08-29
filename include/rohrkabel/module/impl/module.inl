#pragma once

#include "module.hpp"

namespace pipewire::impl
{
    template <detail::listener<mod::raw_type> Listener>
    Listener mod::listen() const
    {
        return {get()};
    }
} // namespace pipewire::impl
