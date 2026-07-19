#pragma once

#include "module.hpp"

namespace pipewire::impl
{
    template <detail::listener<module::raw_type> Listener>
    Listener module::listen() const
    {
        return {get()};
    }
} // namespace pipewire::impl
