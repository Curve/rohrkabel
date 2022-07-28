#include "listener.hpp"
#include "port/port.hpp"

#include <optional>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct port::impl
    {
        pw_port *port;
        port_info info;
    };

    port::~port() = default;

    port::port(port &&port) noexcept : proxy(std::move(port)), m_impl(std::move(port.m_impl)) {}

    port::port(proxy &&_proxy, port_info info) : proxy(std::move(_proxy)), m_impl(std::make_unique<impl>())
    {
        m_impl->info = std::move(info);
        m_impl->port = reinterpret_cast<pw_port *>(proxy::get());
    }

    port &port::operator=(port &&port) noexcept
    {
        proxy::operator=(std::move(port));
        m_impl = std::move(port.m_impl);
        return *this;
    }

    lazy_expected<port> port::bind(pw_port *raw_port)
    {
        struct state
        {
            pw_port_events events;
            std::optional<listener> hook;
            std::promise<port_info> info;
        };

        auto proxy = proxy::bind(reinterpret_cast<pw_proxy *>(raw_port));
        auto m_state = std::make_shared<state>();

        m_state->hook.emplace();
        m_state->events.version = PW_VERSION_PORT_EVENTS;

        m_state->events.info = [](void *data, const pw_port_info *info) {
            auto &m_state = *reinterpret_cast<state *>(data);
            port_info m_info = {info->id,                                     //
                                static_cast<port_direction>(info->direction), //
                                info->change_mask,                            //
                                info->props,                                  //
                                std::vector<param_info>(info->n_params)};

            if (info->params)
            {
                for (auto i = 0u; i < info->n_params; i++)
                {
                    auto param = info->params[i];
                    m_info.params.emplace_back(param_info{param.id, static_cast<param_info_flags>(param.flags)});
                }
            }

            m_state.info.set_value(m_info);
            m_state.hook.reset();
        };

        // NOLINTNEXTLINE
        pw_port_add_listener(raw_port, &m_state->hook->get(), &m_state->events, m_state.get());

        return std::async(std::launch::deferred, [m_state, proxy_fut = std::move(proxy)]() mutable -> tl::expected<port, error> {
            auto proxy = proxy_fut.get();

            if (!proxy.has_value())
            {
                return tl::make_unexpected(proxy.error());
            }

            return port(std::move(proxy.value()), m_state->info.get_future().get());
        });
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