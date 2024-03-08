#pragma once

#include "registry.hpp"

namespace pipewire
{
    template <typename T>
        requires valid_proxy<T>
    lazy<expected<T>> registry::bind(std::uint32_t id, update_strategy strategy)
    {
        using args_t = boost::callable_traits::args_t<decltype(&T::bind)>;
        using raw_t  = std::tuple_element_t<0, args_t>;

        auto rtn = T::bind(reinterpret_cast<raw_t>(bind(id, T::type, T::version)));
        core()->update(strategy);

        return rtn;
    }
} // namespace pipewire
