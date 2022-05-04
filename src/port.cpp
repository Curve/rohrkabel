#include "port/port.hpp"
#include "registry/registry.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct port::impl
    {
        pw_port *port;
        port_info info;
        pw_port_events events;
    };

    port::~port()
    {
        if (m_impl)
        {
            pw_proxy_destroy(reinterpret_cast<pw_proxy *>(m_impl->port));
        }
    }

    port::port(port &&port) noexcept : listener(std::move(port)), m_impl(std::move(port.m_impl)) {}

    port::port(registry &registry, const global &global) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = PW_VERSION_PORT_EVENTS;

        m_impl->events.info = [](void *data, const pw_port_info *info) {
            auto &m_impl = *reinterpret_cast<impl *>(data);
            m_impl.info = {info->id, static_cast<port_direction>(info->direction), info->change_mask, info->props, {}};

            for (auto i = 0u; i < info->n_params; i++)
            {
                auto param = info->params[i];
                m_impl.info.params.emplace_back(param_info{param.id, param.user, param.flags});
            }
        };

        m_impl->port = reinterpret_cast<pw_port *>(pw_registry_bind(registry.get(), global.id, PW_TYPE_INTERFACE_Port, PW_VERSION_PORT, sizeof(void *)));

        // NOLINTNEXTLINE
        pw_port_add_listener(m_impl->port, &listener::get(), &m_impl->events, m_impl.get());
    }

    port_info port::info() const
    {
        return m_impl->info;
    }

    pw_port *port::get() const
    {
        return m_impl->port;
    }
} // namespace pipewire