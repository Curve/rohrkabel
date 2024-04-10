#include "proxy/proxy.hpp"
#include "proxy/events.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct deleter
    {
        void operator()(pw_proxy *proxy)
        {
            pw_proxy_destroy(proxy);
        }
    };

    struct proxy::impl
    {
        spa::dict props;
        std::unique_ptr<pw_proxy, deleter> proxy;
    };

    proxy::~proxy() = default;

    proxy::proxy(proxy &&other) noexcept : m_impl(std::move(other.m_impl)) {}

    proxy::proxy(pw_proxy *proxy, spa::dict props) : m_impl(std::make_unique<impl>())
    {
        m_impl->proxy.reset(proxy);
        m_impl->props = std::move(props);
    }

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

    pw_proxy *proxy::get() const
    {
        return m_impl->proxy.get();
    }

    template <>
    proxy_listener proxy::listen()
    {
        return {get()};
    }

    proxy::operator pw_proxy *() const &
    {
        return get();
    }

    lazy<expected<proxy>> proxy::bind(pw_proxy *raw)
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

            return proxy{raw, std::move(m_state->props)};
        });
    }
} // namespace pipewire
