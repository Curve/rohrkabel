#pragma once

#include "registry.hpp"

#include <boost/callable_traits.hpp>

namespace pipewire
{
    template <class Listener>
        requires valid_listener<Listener, registry::raw_type>
    Listener registry::listen()
    {
        return {get()};
    }

    template <typename T, update_strategy Strategy>
        requires valid_proxy<T>
    lazy<expected<T>> registry::bind(std::uint32_t id)
    {
        using args_t = boost::callable_traits::args_t<decltype(&T::bind)>;
        using raw_t  = std::tuple_element_t<0, args_t>;

        auto rtn = T::bind(reinterpret_cast<raw_t>(bind(id, T::type, T::version)));
        core()->update<Strategy>();

        return rtn;
    }

    template <typename T>
        requires valid_proxy<T>
    lazy<expected<T>> registry::bind(std::uint32_t id, update_strategy strategy)
    {
        if (strategy == update_strategy::none)
        {
            return bind<T, update_strategy::none>(id);
        }

        return bind<T, update_strategy::sync>(id);
    }
} // namespace pipewire
