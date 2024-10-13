#pragma once

#include "core.hpp"

#include <boost/callable_traits.hpp>

namespace pipewire
{
    template <class Listener>
        requires detail::valid_listener<Listener, core::raw_type>
    Listener core::listen()
    {
        return {get()};
    }

    template <typename T>
        requires detail::valid_proxy<T>
    lazy<expected<T>> core::create(factory factory, update_strategy strategy)
    {
        using args_t = boost::callable_traits::args_t<decltype(&T::bind)>;
        using raw_t  = std::tuple_element_t<0, args_t>;

        if (!factory.type)
        {
            factory.type = T::type;
        }

        if (!factory.version)
        {
            factory.version = T::version;
        }

        auto *proxy = create(std::move(factory));
        auto rtn    = T::bind(reinterpret_cast<raw_t>(proxy));

        update(strategy);

        return rtn;
    }
} // namespace pipewire
