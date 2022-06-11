#pragma once
#include "events.hpp"

namespace pipewire
{
    template <auto Val, typename Type> void event<Val, Type>::set(callback_t &&callback)
    {
        m_callback = std::move(callback);
    }

    template <auto Val, typename Type> template <typename... Params> void event<Val, Type>::fire(Params &&...params)
    {
        if (m_callback)
        {
            m_callback(std::forward<Params>(params)...);
        }
    }

    template <class... Events> template <auto Val, std::size_t I> constexpr auto event_handler<Events...>::get()
    {
        if constexpr (I < std::tuple_size_v<tuple_t>)
        {
            using item_t = std::tuple_element_t<I, tuple_t>;

            if constexpr (Val == item_t::value)
            {
                return type_identity<typename item_t::callback_t>();
            }
            else
            {
                return get<Val, I + 1>();
            }
        }
        else
        {
            return type_identity<void>();
        }
    }

    template <class... Events> template <auto Val, std::size_t I> constexpr auto &event_handler<Events...>::at()
    {
        if constexpr (I < std::tuple_size_v<tuple_t>)
        {
            using item_t = std::tuple_element_t<I, tuple_t>;

            if constexpr (Val == item_t::value)
            {
                return std::get<I>(m_events);
            }
            else
            {
                return at<Val, I + 1>();
            }
        }
    }

} // namespace pipewire