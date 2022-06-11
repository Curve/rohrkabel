#pragma once
#include "../type_indentity.hpp"
#include <functional>

namespace pipewire
{
    template <auto Val, typename Type> struct event
    {
        static constexpr auto value = Val;
        using callback_t = std::function<Type>;

      private:
        callback_t m_callback;

      public:
        void set(callback_t &&);
        template <typename... Params> void fire(Params &&...);
    };

    template <typename> struct is_event : std::false_type
    {
    };

    template <auto Val, typename Type> struct is_event<event<Val, Type>> : std::true_type
    {
    };

    template <class... Events> class event_handler
    {
        static_assert((is_event<Events>::value && ...));
        using tuple_t = std::tuple<Events...>;

      private:
        tuple_t m_events;

      public:
        template <auto Val, std::size_t I = 0> static constexpr auto get();
        template <auto Val> using get_t = typename decltype(get<Val>())::type;

      public:
        template <auto Val, std::size_t I = 0> constexpr auto &at();
    };
} // namespace pipewire

#include "events.inl"