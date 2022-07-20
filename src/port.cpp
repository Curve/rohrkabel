#include "listener.hpp"
#include "port/port.hpp"
#include "registry/registry.hpp"

#include <optional>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct port::impl
    {
        pw_port *port;
        port_info info;
        pw_port_events events;
        std::optional<listener> hook;
    };

    port::~port() = default;

    port::port(pw_port *port) : proxy(reinterpret_cast<pw_proxy *>(port)), m_impl(std::make_unique<impl>())
    {
        m_impl->port = port;
        m_impl->events.version = PW_VERSION_PORT_EVENTS;

        m_impl->events.info = [](void *data, const pw_port_info *info) {
            auto &m_impl = *reinterpret_cast<impl *>(data);
            m_impl.info = {info->id, static_cast<port_direction>(info->direction), info->change_mask, info->props, std::vector<param_info>(info->n_params)};

            if (info->params)
            {
                for (auto i = 0u; i < info->n_params; i++)
                {
                    auto param = info->params[i];
                    m_impl.info.params.emplace_back(param_info{param.id, static_cast<param_info_flags>(param.flags)});
                }
            }

            m_impl.hook.reset();
        };

        m_impl->hook.emplace();

        // NOLINTNEXTLINE
        pw_port_add_listener(m_impl->port, &m_impl->hook->get(), &m_impl->events, m_impl.get());
    }

    port::port(port &&port) noexcept : proxy(std::move(port)), m_impl(std::move(port.m_impl)) {}

    port::port(registry &registry, std::uint32_t id) : port(reinterpret_cast<pw_port *>(pw_registry_bind(registry.get(), id, type.c_str(), version, sizeof(void *)))) {}

    port &port::operator=(port &&port) noexcept
    {
        proxy::operator=(std::move(port));
        m_impl = std::move(port.m_impl);
        return *this;
    }

    port_info port::info() const
    {
        return m_impl->info;
    }

    port::params_t port::params() const
    {
        struct state
        {
            listener hook;
            pw_port_events events;
            std::map<std::uint32_t, spa::pod> params;
        };

        auto m_state = std::make_shared<state>();
        m_state->events.version = PW_VERSION_PORT_EVENTS;

        for (const auto &param : m_impl->info.params)
        {
            // NOLINTNEXTLINE
            pw_port_enum_params(m_impl->port, 0, param.id, 0, 1, nullptr);
        }

        m_state->events.param = [](void *data, int, uint32_t id, uint32_t, uint32_t, const struct spa_pod *param) {
            auto &m_state = *reinterpret_cast<state *>(data);
            m_state.params.emplace(id, param);
        };

        // NOLINTNEXTLINE
        pw_port_add_listener(m_impl->port, &m_state->hook.get(), &m_state->events, m_state.get());
        return std::async(std::launch::deferred, [m_state] { return std::map<std::uint32_t, spa::pod>(std::move(m_state->params)); });
    }

    pw_port *port::get() const
    {
        return m_impl->port;
    }

    const std::string port::type = PW_TYPE_INTERFACE_Port;
    const std::uint32_t port::version = PW_VERSION_PORT;
} // namespace pipewire