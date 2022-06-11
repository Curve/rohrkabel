#pragma once
#include "../global.hpp"
#include "../listener.hpp"
#include "../utils/events/events.hpp"

#include <memory>
#include <cstdint>

namespace pipewire
{
    enum class registry_event
    {
        global,
        global_removed
    };

    class registry;
    class registry_listener : listener
    {
        struct impl;

        using events_t = event_handler<                                //
            event<registry_event::global, void(const global &)>,       //
            event<registry_event::global_removed, void(std::uint32_t)> //
            >;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~registry_listener();

      public:
        registry_listener(const registry &);
        registry_listener(registry_listener &&) noexcept;

      public:
        template <registry_event Event> void on(events_t::get_t<Event> &&) = delete;
    };

    template <> void registry_listener::on<registry_event::global>(events_t::get_t<registry_event::global> &&);
    template <> void registry_listener::on<registry_event::global_removed>(events_t::get_t<registry_event::global_removed> &&);

} // namespace pipewire