#pragma once
#include "core.hpp"
#include "../utils/traits.hpp"

#include <boost/callable_traits.hpp>

namespace pipewire
{
    template <typename T, typename Factory>
    lazy<expected<T>> core::create(const Factory &factory, update_strategy strategy)
    {
        static_assert(std::same_as<Factory, pipewire::factory>, "The supplied factory is not supported for this type");
        static_assert(valid_proxy<T>, "The supplied class is not a valid proxy");

        using args_t = boost::callable_traits::args_t<decltype(&T::bind)>;
        using raw_t  = std::tuple_element_t<0, args_t>;

        auto *dict   = &factory.props.get()->dict;
        auto type    = factory.type.value_or(T::type);
        auto version = factory.version.value_or(T::version);

        auto *proxy = pw_core_create_object(get(), factory.name.c_str(), type.c_str(), version, dict, 0);
        auto rtn    = T::bind(reinterpret_cast<raw_t>(proxy));

        update(strategy);

        return rtn;
    }
} // namespace pipewire