#include "listener.hpp"
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
        std::unique_ptr<listener> hook;

        std::uint32_t last_id;
        std::map<std::uint32_t, spa::pod> params;
    };

    port::~port()
    {
        if (m_impl)
        {
            pw_proxy_destroy(reinterpret_cast<pw_proxy *>(m_impl->port));
        }
    }

    port::port(port &&port) noexcept : m_impl(std::move(port.m_impl)) {}

    port::port(registry &registry, const global &global) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = PW_VERSION_PORT_EVENTS;

        m_impl->events.info = [](void *data, const pw_port_info *info) {
            auto &m_impl = *reinterpret_cast<impl *>(data);
            m_impl.info = {info->id, static_cast<port_direction>(info->direction), info->change_mask, info->props};

            if (info->params)
            {
                m_impl.last_id = info->params[info->n_params - 1].id;

                for (auto i = 0u; i < info->n_params; i++)
                {
                    auto param = info->params[i];

                    if (param.flags & SPA_PARAM_INFO_READ)
                    {
                        // NOLINTNEXTLINE
                        pw_port_enum_params(m_impl.port, 0, param.id, 0, -1, nullptr);
                    }
                }
            }
        };
        m_impl->events.param = [](void *data, int, uint32_t id, uint32_t, uint32_t, const struct spa_pod *param) {
            auto &m_impl = *reinterpret_cast<impl *>(data);
            m_impl.params.emplace(id, param);

            if (id == m_impl.last_id)
            {
                m_impl.hook.reset();
            }
        };

        m_impl->hook = std::make_unique<listener>();
        m_impl->port = reinterpret_cast<pw_port *>(pw_registry_bind(registry.get(), global.id, type.c_str(), version, sizeof(void *)));

        // NOLINTNEXTLINE
        pw_port_add_listener(m_impl->port, &m_impl->hook->get(), &m_impl->events, m_impl.get());
    }

    port &port::operator=(port &&port) noexcept
    {
        m_impl = std::move(port.m_impl);
        return *this;
    }

    port_info port::info() const
    {
        return m_impl->info;
    }

    const std::map<std::uint32_t, spa::pod> &port::params() const
    {
        return m_impl->params;
    }

    pw_port *port::get() const
    {
        return m_impl->port;
    }

    const std::string port::type = PW_TYPE_INTERFACE_Port;
    const std::uint32_t port::version = PW_VERSION_PORT;
} // namespace pipewire