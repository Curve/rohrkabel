#include "client/client.hpp"
#include "client/events.hpp"

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

    template <>
    client_listener client::listen()
    {
        return {get()};
    }

    client::operator pw_client *() const &
    {
        return get();
    }

    lazy<expected<client>> client::bind(pw_client *raw)
    {
        struct state
        {
            client_listener events;

          public:
            std::promise<client_info> info;
        };

        auto proxy = proxy::bind(reinterpret_cast<pw_proxy *>(raw));

        auto m_state    = std::make_shared<state>(raw);
        auto weak_state = std::weak_ptr{m_state};

        m_state->events.once<client_event::info>([weak_state](client_info info) {
            weak_state.lock()->info.set_value(std::move(info));
        });

        return make_lazy<expected<client>>([m_state, fut = std::move(proxy)]() mutable -> expected<client> {
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
