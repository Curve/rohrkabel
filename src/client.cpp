#include "client/client.hpp"
#include "client/events.hpp"

#include <pipewire/pipewire.h>
#include <coco/promise/promise.hpp>

namespace pipewire
{
    struct client::impl
    {
        raw_type *client;
        client_info info;
    };

    client::client(proxy &&base, client_info info) : proxy(std::move(base)), m_impl(std::make_unique<impl>())
    {
        m_impl->client = reinterpret_cast<raw_type *>(proxy::get());
        m_impl->info   = std::move(info);
    }

    client::client(client &&) noexcept = default;

    client &client::operator=(client &&) noexcept = default;

    client::~client() = default;

    client_info client::info() const
    {
        return m_impl->info;
    }

    client::raw_type *client::get() const
    {
        return m_impl->client;
    }

    client::raw_type *client::release() &&
    {
        std::ignore = std::move(*this).proxy::release();
        return std::exchange(m_impl->client, nullptr);
    }

    client::operator raw_type *() const &
    {
        return get();
    }

    task<client> client::bind(raw_type *raw)
    {
        auto _proxy   = proxy::bind(reinterpret_cast<proxy::raw_type *>(raw));
        auto listener = client_listener{raw};

        auto promise = coco::promise<client_info>{};
        auto fut     = promise.get_future();

        listener.once<client_event::info>(
            [promise = std::move(promise)](client_info info) mutable
            {
                promise.set_value(std::move(info));
            });

        co_await task<client>::make_lazy{};
        auto proxy = co_await std::move(_proxy);

        if (!proxy.has_value())
        {
            co_return std::unexpected{proxy.error()};
        }

        co_return client{std::move(proxy.value()), co_await std::move(fut)};
    }

    const char *client::type            = PW_TYPE_INTERFACE_Client;
    const std::uint32_t client::version = PW_VERSION_CLIENT;
} // namespace pipewire
