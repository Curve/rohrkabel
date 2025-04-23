#pragma once

#include "core.hpp"

namespace pipewire
{
    template <detail::Listener<core::raw_type> Listener>
    Listener core::listen() const
    {
        return {get()};
    }

    template <detail::Proxy T>
    task<T> core::create(factory factory)
    {
        using raw_t = std::add_pointer_t<typename T::raw_type>;

        if (!factory.type)
        {
            factory.type = T::type;
        }

        if (!factory.version)
        {
            factory.version = T::version;
        }

        return T::bind(reinterpret_cast<raw_t>(create_object(std::move(factory))));
    }
} // namespace pipewire
