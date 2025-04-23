#include "port/port.hpp"
#include "port/events.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct port_listener::impl
    {
        pw_port_events events;
    };

    port_listener::port_listener(port::raw_type *port) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = version;

        m_impl->events.info = [](void *data, const pw_port_info *info) {
            auto &events = *reinterpret_cast<listener::events *>(data);
            events.get<port_event::info>().fire(port_info::from(info));
        };

        m_impl->events.param = [](void *data, int seq, uint32_t id, uint32_t index, uint32_t next, const struct spa_pod *param) {
            auto &events = *reinterpret_cast<listener::events *>(data);
            events.get<port_event::param>().fire(seq, id, index, next, spa::pod::copy(param));
        };

        pw_port_add_listener(port, listener::get(), &m_impl->events, m_events.get());
    }

    port_listener::~port_listener() = default;

    const std::uint32_t port_listener::version = PW_VERSION_PORT_EVENTS;
} // namespace pipewire
