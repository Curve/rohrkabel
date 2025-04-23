#pragma once

#include "../spa/hook.hpp"

#include <ereignis/manager/manager.hpp>

namespace pipewire
{
    template <ereignis::Event... Events>
    struct listener : spa::hook
    {
        using events = ereignis::manager<Events...>;

      protected:
        std::unique_ptr<events> m_events;

      public:
        listener() : spa::hook(spa::hook::create()), m_events(std::make_unique<events>()) {}

      public:
        template <typename T>
        void clear(T event)
        {
            m_events->clear(event);
        }

        template <typename T>
        void remove(T event, std::uint64_t id)
        {
            m_events->remove(event, id);
        }

      public:
        template <auto Event>
        std::uint64_t on(events::template event<Event>::callback &&callback)
        {
            return m_events->template get<Event>().add(std::move(callback));
        }

        template <auto Event>
        void once(events::template event<Event>::callback &&callback)
        {
            m_events->template get<Event>().once(std::move(callback));
        }
    };
} // namespace pipewire
