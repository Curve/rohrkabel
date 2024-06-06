#include "proxy/proxy.hpp"
#include "proxy/events.hpp"
#include "utils/deleter.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct proxy::impl
    {
        spa::dict props;
        pw_unique_ptr<raw_type> proxy;
    };

    proxy::~proxy() = default;

    proxy::proxy(deleter<raw_type> deleter, raw_type *raw, spa::dict dict)
        : m_impl(std::make_unique<impl>(std::move(dict), pw_unique_ptr<raw_type>{raw, deleter}))
    {
    }

    proxy::proxy(proxy &&other) noexcept : m_impl(std::move(other.m_impl)) {}

    proxy &proxy::operator=(proxy &&other) noexcept
    {
        m_impl = std::move(other.m_impl);
        return *this;
    }

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

    lazy<expected<proxy>> proxy::bind(raw_type *raw)
    {
        struct state
        {
            proxy_listener listener;

          public:
            spa::dict props;
            std::promise<expected<void>> done;
        };

        auto m_state    = std::make_shared<state>(raw);
        auto weak_state = std::weak_ptr{m_state};

        m_state->listener.once<proxy_event::bound>([weak_state](std::uint32_t) {
            weak_state.lock()->done.set_value({});
        });

        m_state->listener.once<proxy_event::bound_props>([weak_state](std::uint32_t, spa::dict props) {
            weak_state.lock()->props = std::move(props);
        });

        m_state->listener.once<proxy_event::error>([weak_state](int seq, int res, const char *message) {
            weak_state.lock()->done.set_value(tl::make_unexpected<error>({seq, res, message}));
        });

        return make_lazy<expected<proxy>>([m_state, raw]() -> expected<proxy> {
            auto done = m_state->done.get_future().get();

            if (!done.has_value())
            {
                return tl::make_unexpected(done.error());
            }

            return from(raw, std::move(m_state->props));
        });
    }

    proxy proxy::from(raw_type *raw, spa::dict props)
    {
        static constexpr auto deleter = [](auto *proxy) {
            pw_proxy_destroy(proxy);
        };

        return {deleter, raw, std::move(props)};
    }

    proxy proxy::view(raw_type *raw, spa::dict props)
    {
        return {view_deleter<raw_type>, raw, std::move(props)};
    }
} // namespace pipewire
