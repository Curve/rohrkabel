#include "listener.hpp"
#include "client/client.hpp"
#include "registry/registry.hpp"

#include <optional>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct client::impl
    {
        client_info info;
        pw_client *client;
        pw_client_events events;
        std::optional<listener> hook;
    };

    client::~client()
    {
        if (m_impl)
        {
            pw_proxy_destroy(reinterpret_cast<pw_proxy *>(m_impl->client));
        }
    }

    client::client(client &&client) noexcept : m_impl(std::move(client.m_impl)) {}

    client::client(registry &registry, std::uint32_t id) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = PW_VERSION_CLIENT_EVENTS;

        m_impl->events.info = [](void *data, const pw_client_info *info) {
            auto &m_impl = *reinterpret_cast<impl *>(data);
            m_impl.info = {info->props, info->id, info->change_mask};
            m_impl.hook.reset();
        };

        m_impl->hook.emplace();
        m_impl->client = reinterpret_cast<pw_client *>(pw_registry_bind(registry.get(), id, type.c_str(), version, sizeof(void *)));

        // NOLINTNEXTLINE
        pw_client_add_listener(m_impl->client, &m_impl->hook->get(), &m_impl->events, m_impl.get());
    }

    client &client::operator=(client &&client) noexcept
    {
        m_impl = std::move(client.m_impl);
        return *this;
    }

    client_info client::info() const
    {
        return m_impl->info;
    }

    pw_client *client::get() const
    {
        return m_impl->client;
    }

    const std::string client::type = PW_TYPE_INTERFACE_Client;
    const std::uint32_t client::version = PW_VERSION_CLIENT;
} // namespace pipewire