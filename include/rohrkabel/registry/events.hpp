#pragma once

#include "../global.hpp"
#include "../listener.hpp"

#include <memory>
#include <cstdint>
#include <ereignis/manager.hpp>

namespace pipewire
{
    enum class registry_event : std::uint8_t
    {
        global,
        global_removed
    };

    class registry;

    class registry_listener : listener
    {
        struct impl;

      private:
        using events = ereignis::manager<                                        //
            ereignis::event<registry_event::global, void(const global &)>,       //
            ereignis::event<registry_event::global_removed, void(std::uint32_t)> //
            >;

      private:
        events m_events;
        std::unique_ptr<impl> m_impl;

      public:
        ~registry_listener();

      public:
        registry_listener(const registry &);
        registry_listener(registry_listener &&) noexcept;

      public:
        void clear(registry_event event);
        void remove(registry_event event, std::uint64_t id);

      public:
        template <registry_event Event>
        std::uint64_t on(events::type_t<Event> &&) = delete;
    };

    template <>
    std::uint64_t registry_listener::on<registry_event::global>(events::type_t<registry_event::global> &&);
    template <>
    std::uint64_t
    registry_listener::on<registry_event::global_removed>(events::type_t<registry_event::global_removed> &&);

} // namespace pipewire
