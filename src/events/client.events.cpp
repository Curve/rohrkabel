#include "client/events.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct client_listener::impl
    {
        pw_client_events events;
    };

    client_listener::client_listener(client::raw_type *client) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = version;

        m_impl->events.info = [](void *data, const pw_client_info *info)
        {
            auto &events = *reinterpret_cast<listener::events *>(data);
            events.get<client_event::info>().fire(client_info::from(info));
        };

        m_impl->events.permissions = [](void *data, std::uint32_t index, std::uint32_t count, const pw_permission *permission)
        {
            auto &events = *reinterpret_cast<listener::events *>(data);
            events.get<client_event::permission>().fire(index, count, permission);
        };

        pw_client_add_listener(client, listener::get(), &m_impl->events, m_events.get());
    }

    client_listener::~client_listener() = default;

    const std::uint32_t client_listener::version = PW_VERSION_CLIENT_EVENTS;
} // namespace pipewire
