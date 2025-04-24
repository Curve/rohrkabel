#pragma once

#include "registry.hpp"

namespace pipewire
{
    template <detail::Listener<registry::raw_type> Listener>
    Listener registry::listen() const
    {
        return {get()};
    }

    template <detail::Proxy T>
    task<T> registry::bind(std::uint32_t id)
    {
        using raw_t = std::add_pointer_t<typename T::raw_type>;
        auto rtn    = T::bind(reinterpret_cast<raw_t>(bind(id, T::type, T::version)));

        if constexpr (detail::sync_after_bind<T>)
        {
            co_await core()->sync();
        }

        co_return co_await std::move(rtn);
    }
} // namespace pipewire
