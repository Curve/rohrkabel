#include "client/events.hpp"
#include "client/client.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct client_listener::impl
    {
        pw_client_events events;
    };

    client_listener::~client_listener() = default;

    client_listener::client_listener(client::raw_type *client) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = PW_VERSION_CLIENT_EVENTS;

        m_impl->events.info = [](void *data, const pw_client_info *info) {
            auto &thiz = *reinterpret_cast<client_listener *>(data);
            thiz.m_events.at<client_event::info>().fire(client_info::from(info));
        };

        m_impl->events.permissions = [](void *data, std::uint32_t index, std::uint32_t count,
                                        const pw_permission *permission) {
            auto &thiz = *reinterpret_cast<client_listener *>(data);
            thiz.m_events.at<client_event::permission>().fire(index, count, permission);
        };

        pw_client_add_listener(client, listener::get(), &m_impl->events, this);
    }

    client_listener::client_listener(client_listener &&client_listener) noexcept
        : listener(std::move(client_listener)), m_impl(std::move(client_listener.m_impl))
    {
    }
} // namespace pipewire
