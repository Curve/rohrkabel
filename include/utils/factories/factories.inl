#pragma once
#include "factories.hpp"

namespace pipewire
{
    template <class... Factories> template <typename Type, std::size_t I> constexpr auto factory_handler<Factories...>::get()
    {
        if constexpr (I < std::tuple_size_v<tuple_t>)
        {
            using item_t = std::tuple_element_t<I, tuple_t>;
            if constexpr (std::is_same_v<typename item_t::factory_t, Type>)
            {
                return type_identity<typename item_t::args_t>();
            }
            else
            {
                return get<Type, I + 1>();
            }
        }
        else
        {
            return type_identity<void>();
        }
    }
} // namespace pipewire