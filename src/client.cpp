#include "listener.hpp"
#include "client/client.hpp"

#include <optional>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct client::impl
    {
        pw_client *client;
        client_info info;
    };

    client::~client() = default;

    client::client(client &&client) noexcept : proxy(std::move(client)), m_impl(std::move(client.m_impl)) {}

    client::client(proxy &&_proxy, client_info info) : proxy(std::move(_proxy)), m_impl(std::make_unique<impl>())
    {
        m_impl->info = std::move(info);
        m_impl->client = reinterpret_cast<pw_client *>(proxy::get());
    }

    lazy_expected<client> client::bind(pw_client *raw_client)
    {
        struct state
        {
            pw_client_events events;
            std::optional<listener> hook;
            std::promise<client_info> info;
        };

        auto proxy = proxy::bind(reinterpret_cast<pw_proxy *>(raw_client));
        auto m_state = std::make_shared<state>();

        m_state->hook.emplace();
        m_state->events.version = PW_VERSION_CLIENT_EVENTS;

        m_state->events.info = [](void *data, const pw_client_info *info) {
            auto &m_state = *reinterpret_cast<state *>(data);

            m_state.info.set_value({info->props, info->id, info->change_mask});
            m_state.hook.reset();
        };

        // NOLINTNEXTLINE
        pw_client_add_listener(raw_client, &m_state->hook->get(), &m_state->events, m_state.get());

        return std::async(std::launch::deferred, [m_state, proxy_fut = std::move(proxy)]() mutable -> tl::expected<client, error> {
            auto proxy = proxy_fut.get();

            if (!proxy.has_value())
            {
                return tl::make_unexpected(proxy.error());
            }

            return client(std::move(proxy.value()), m_state->info.get_future().get());
        });
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