#include "proxy/proxy.hpp"
#include "proxy/events.hpp"

#include <pipewire/pipewire.h>
#include <coco/promise/promise.hpp>

namespace pipewire
{
    struct proxy::impl
    {
        spa::dict props;
        pw_unique_ptr<raw_type> proxy;
    };

    proxy::proxy(deleter<raw_type> deleter, raw_type *raw, spa::dict dict)
        : m_impl(std::make_unique<impl>(std::move(dict), pw_unique_ptr<raw_type>{raw, deleter}))
    {
    }

    proxy::proxy(proxy &&) noexcept = default;

    proxy &proxy::operator=(proxy &&) noexcept = default;

    proxy::~proxy() = default;

    spa::dict proxy::props() const
    {
        return m_impl->props;
    }

    std::string proxy::type() const
    {
        return pw_proxy_get_type(m_impl->proxy.get(), nullptr);
    }

    std::uint32_t proxy::id() const
    {
        return pw_proxy_get_bound_id(m_impl->proxy.get());
    }

    std::uint32_t proxy::version() const
    {
        std::uint32_t version{};
        static_cast<void>(pw_proxy_get_type(m_impl->proxy.get(), &version));

        return version;
    }

    proxy::raw_type *proxy::get() const
    {
        return m_impl->proxy.get();
    }

    proxy::operator raw_type *() const &
    {
        return get();
    }

    task<proxy> proxy::bind(raw_type *raw)
    {
        auto listener = proxy_listener{raw};
        auto props    = spa::dict{};

        auto done = coco::promise<expected<void>>{};
        auto fut  = done.get_future();

        listener.once<proxy_event::bound>(
            [&](std::uint32_t)
            {
                done.set_value({});
            });

        listener.once<proxy_event::bound_props>(
            [&](std::uint32_t, spa::dict properties)
            {
                props = std::move(properties);
            });

        listener.once<proxy_event::error>(
            [&](int seq, int res, const char *message)
            {
                done.set_value(std::unexpected<error>{{
                    .seq     = seq,
                    .res     = res,
                    .message = message,
                }});
            });

        co_await task<proxy>::wake_on_await{};
        auto result = co_await std::move(fut);

        if (!result.has_value())
        {
            co_return std::unexpected{result.error()};
        }

        co_return from(raw, std::move(props));
    }

    proxy proxy::from(raw_type *raw, spa::dict props)
    {
        return {pw_proxy_destroy, raw, std::move(props)};
    }

    proxy proxy::view(raw_type *raw, spa::dict props)
    {
        return {view_deleter<raw_type>, raw, std::move(props)};
    }
} // namespace pipewire
