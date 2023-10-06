#pragma once
#include "core.hpp"
#include "../utils/traits.hpp"

#include <boost/callable_traits.hpp>

namespace pipewire
{
    template <typename T, typename Factory>
    lazy<expected<T>> core::create(Factory factory, update_strategy strategy)
    {
        static_assert(std::same_as<Factory, pipewire::factory>, "The supplied factory is not supported for this type");
        static_assert(valid_proxy<T>, "The supplied class is not a valid proxy");

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

        auto proxy = create(std::move(factory));
        auto rtn   = T::bind(reinterpret_cast<raw_t>(proxy));

        update(strategy);

        return rtn;
    }
} // namespace pipewire