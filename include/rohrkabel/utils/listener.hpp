#pragma once

#include "../spa/hook.hpp"

#include <ereignis/manager.hpp>

namespace pipewire
{
    template <typename Listener, typename Raw>
    concept valid_listener = requires() { requires std::constructible_from<Listener, Raw *>; };

    template <typename T, ereignis::ereignis_event... Events>
    struct listener : public spa::hook
    {
        using events = ereignis::manager<Events...>;

      protected:
        std::unique_ptr<events> m_events;

      public:
        ~listener() = default;

      public:
        listener() : m_events(std::make_unique<events>()) {}
        listener(listener &&other) noexcept : spa::hook(std::move(other)), m_events(std::move(other.m_events)) {}

      public:
        void clear(T event)
        {
            m_events.clear(event);
        }

        void remove(T event, std::uint64_t id)
        {
            m_events.remove(event, id);
        }

      public:
        template <T Event>
        std::uint64_t on(events::template type_t<Event> &&callback)
        {
            return m_events->template at<Event>().add(std::move(callback));
        }

        template <T Event>
        void once(events::template type_t<Event> &&callback)
        {
            m_events->template at<Event>().once(std::move(callback));
        }

      public:
        using spa::hook::get;
    };
} // namespace pipewire
