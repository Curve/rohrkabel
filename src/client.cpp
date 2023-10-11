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

    client::client(client &&other) noexcept : proxy(std::move(other)), m_impl(std::move(other.m_impl)) {}

    client::client(proxy &&base, client_info info) : proxy(std::move(base)), m_impl(std::make_unique<impl>())
    {
        m_impl->client = reinterpret_cast<pw_client *>(proxy::get());
        m_impl->info   = std::move(info);
    }

    client &client::operator=(client &&other) noexcept
    {
        proxy::operator=(std::move(other));
        m_impl = std::move(other.m_impl);
        return *this;
    }

    pw_client *client::get() const
    {
        return m_impl->client;
    }

    client_info client::info() const
    {
        return m_impl->info;
    }

    client::operator pw_client *() const &
    {
        return get();
    }

    lazy<expected<client>> client::bind(pw_client *raw)
    {
        struct state
        {
            pw_client_events events;
            std::optional<listener> hook;
            std::promise<client_info> info;
        };

        auto proxy   = proxy::bind(reinterpret_cast<pw_proxy *>(raw));
        auto m_state = std::make_shared<state>();

        m_state->hook.emplace();
        m_state->events.version = PW_VERSION_CLIENT_EVENTS;

        m_state->events.info = [](void *data, const pw_client_info *info)
        {
            auto &m_state = *reinterpret_cast<state *>(data);

            m_state.info.set_value({info->props, info->id, info->change_mask});
            m_state.hook.reset();
        };

        // NOLINTNEXTLINE(*-optional-access)
        pw_client_add_listener(raw, m_state->hook->get(), &m_state->events, m_state.get());

        return make_lazy<expected<client>>(
            [m_state, fut = std::move(proxy)]() mutable -> expected<client>
            {
                auto proxy = fut.get();

                if (!proxy.has_value())
                {
                    return tl::make_unexpected(proxy.error());
                }

                return client{std::move(proxy.value()), m_state->info.get_future().get()};
            });
    }

    const char *client::type            = PW_TYPE_INTERFACE_Client;
    const std::uint32_t client::version = PW_VERSION_CLIENT;
} // namespace pipewire
