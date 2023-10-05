#pragma once
#include "info.hpp"
#include "../error.hpp"
#include "../listener.hpp"

#include <memory>
#include <ereignis/manager.hpp>

namespace pipewire
{
    enum class core_event : std::uint8_t
    {
        info,
        done,
        error,
    };

    class core;

    class core_listener : listener
    {
        struct impl;

      private:
        using events = ereignis::manager<                                          //
            ereignis::event<core_event::info, void(const core_info &)>,            //
            ereignis::event<core_event::done, void(std::uint32_t, int)>,           //
            ereignis::event<core_event::error, void(std::uint32_t, const error &)> //
            >;

      private:
        events m_events;
        std::unique_ptr<impl> m_impl;

      public:
        ~core_listener();

      public:
        core_listener(const core &);
        core_listener(core_listener &&) noexcept;

      public:
        void clear(core_event event);
        void remove(core_event event, std::uint64_t id);

      public:
        template <core_event Event>
        std::uint64_t on(events::type_t<Event> &&) = delete;
    };

    template <>
    std::uint64_t core_listener::on<core_event::info>(events::type_t<core_event::info> &&);
    template <>
    std::uint64_t core_listener::on<core_event::done>(events::type_t<core_event::done> &&);
    template <>
    std::uint64_t core_listener::on<core_event::error>(events::type_t<core_event::error> &&);
} // namespace pipewire