#include "port/port.hpp"
#include "port/events.hpp"

#include <pipewire/pipewire.h>
#include <coco/promise/promise.hpp>

namespace pipewire
{
    struct port::impl
    {
        raw_type *port;
        port_info info;
    };

    port::port(proxy &&base, port_info info) : proxy(std::move(base)), m_impl(std::make_unique<impl>())
    {
        m_impl->port = reinterpret_cast<raw_type *>(proxy::get());
        m_impl->info = std::move(info);
    }

    port::port(port &&) noexcept = default;

    port &port::operator=(port &&) noexcept = default;

    port::~port() = default;

    port_info port::info() const
    {
        return m_impl->info;
    }

    lazy<port::params_t> port::params() const
    {
        auto listener = listen();
        auto params   = params_t{};

        listener.on<port_event::param>(
            [&](auto, auto id, auto, auto, spa::pod param)
            {
                params.emplace(id, std::move(param));
            });

        for (const auto &param : m_impl->info.params)
        {
            pw_port_enum_params(m_impl->port, 0, param.id, 0, 1, nullptr);
        }

        co_await lazy<params_t>::make_lazy{};
        co_return params;
    }

    port::raw_type *port::get() const
    {
        return m_impl->port;
    }

    port::raw_type *port::release() &&
    {
        std::ignore = std::move(*this).proxy::release();
        return std::exchange(m_impl->port, nullptr);
    }

    port::operator raw_type *() const &
    {
        return get();
    }

    task<port> port::bind(raw_type *raw)
    {
        auto _proxy   = proxy::bind(reinterpret_cast<proxy::raw_type *>(raw));
        auto listener = port_listener{raw};

        auto promise = coco::promise<port_info>{};
        auto fut     = promise.get_future();

        listener.once<port_event::info>(
            [promise = std::move(promise)](port_info info) mutable
            {
                promise.set_value(std::move(info));
            });

        co_await task<port>::make_lazy{};
        auto proxy = co_await std::move(_proxy);

        if (!proxy.has_value())
        {
            co_return std::unexpected{proxy.error()};
        }

        co_return port{std::move(proxy.value()), co_await std::move(fut)};
    }

    const char *port::type            = PW_TYPE_INTERFACE_Port;
    const std::uint32_t port::version = PW_VERSION_PORT;
} // namespace pipewire
