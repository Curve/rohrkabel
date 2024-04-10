#pragma once

#include "proxy.hpp"

#include "../listener.hpp"
#include "../spa/dict.hpp"

#include <memory>
#include <ereignis/manager.hpp>

namespace pipewire
{
    enum class proxy_event : std::uint8_t
    {
        error,
        bound,
        bound_props,
    };

    class proxy_listener : listener
    {
        struct impl;

      private:
        using events = ereignis::manager<                                             //
            ereignis::event<proxy_event::error, void(int, int, const char *)>,        //
            ereignis::event<proxy_event::bound, void(std::uint32_t)>,                 //
            ereignis::event<proxy_event::bound_props, void(std::uint32_t, spa::dict)> //
            >;

      private:
        events m_events;
        std::unique_ptr<impl> m_impl;

      public:
        ~proxy_listener();

      public:
        proxy_listener(pw_proxy *);
        proxy_listener(proxy_listener &&) noexcept;

      public:
        void clear(proxy_event event);
        void remove(proxy_event event, std::uint64_t id);

      public:
        template <proxy_event Event>
        std::uint64_t on(events::type_t<Event> &&) = delete;
    };

    template <>
    std::uint64_t proxy_listener::on<proxy_event::error>(events::type_t<proxy_event::error> &&);
    template <>
    std::uint64_t proxy_listener::on<proxy_event::bound>(events::type_t<proxy_event::bound> &&);
    template <>
    std::uint64_t proxy_listener::on<proxy_event::bound_props>(events::type_t<proxy_event::bound_props> &&);
} // namespace pipewire
