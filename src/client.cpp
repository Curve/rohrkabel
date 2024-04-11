#include "client/client.hpp"
#include "client/events.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct client::impl
    {
        raw_type *client;
        client_info info;
    };

    client::~client() = default;

    client::client(client &&other) noexcept : proxy(std::move(other)), m_impl(std::move(other.m_impl)) {}

    client::client(proxy &&base, client_info info) : proxy(std::move(base)), m_impl(std::make_unique<impl>())
    {
        m_impl->client = reinterpret_cast<raw_type *>(proxy::get());
        m_impl->info   = std::move(info);
    }

    client &client::operator=(client &&other) noexcept
    {
        proxy::operator=(std::move(other));
        m_impl = std::move(other.m_impl);
        return *this;
    }

    client::raw_type *client::get() const
    {
        return m_impl->client;
    }

    client_info client::info() const
    {
        return m_impl->info;
    }

    template client_listener client::listen<client_listener>();

    template <class Listener>
        requires valid_listener<Listener, client::raw_type>
    Listener client::listen()
    {
        return {get()};
    }

    client::operator raw_type *() const &
    {
        return get();
    }

    lazy<expected<client>> client::bind(raw_type *raw)
    {
        struct state
        {
            client_listener events;

          public:
            std::promise<client_info> info;
        };

        auto proxy = proxy::bind(reinterpret_cast<proxy::raw_type *>(raw));

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
