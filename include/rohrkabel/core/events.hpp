#pragma once
#include <memory>
#include "info.hpp"
#include "../listener.hpp"
#include "../utils/events/events.hpp"

namespace pipewire
{
    enum class core_event
    {
        info,
        done,
        error,
    };

    class core;
    class core_listener : listener
    {
        struct impl;

        using events_t = event_handler<event<core_event::info, void(const core_info &)>,                            //
                                       event<core_event::done, void(std::uint32_t, int)>,                           //
                                       event<core_event::error, void(std::uint32_t, int, int, const std::string &)> //
                                       >;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~core_listener();

      public:
        core_listener(const core &);
        core_listener(core_listener &&) noexcept;

      public:
        template <core_event Event> void on(events_t::get_t<Event> &&) = delete;
    };

    template <> void core_listener::on<core_event::info>(events_t::get_t<core_event::info> &&);
    template <> void core_listener::on<core_event::done>(events_t::get_t<core_event::done> &&);
    template <> void core_listener::on<core_event::error>(events_t::get_t<core_event::error> &&);
} // namespace pipewire