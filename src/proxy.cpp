#include "proxy.hpp"
#include "listener.hpp"

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
        std::unique_ptr<pw_proxy, deleter> proxy;
    };

    proxy::~proxy() = default;

    proxy::proxy(pw_proxy *proxy) : m_impl(std::make_unique<impl>())
    {
        m_impl->proxy.reset(proxy);
    }

    proxy::proxy(proxy &&other) noexcept : m_impl(std::move(other.m_impl)) {}

    proxy &proxy::operator=(proxy &&other) noexcept
    {
        m_impl = std::move(other.m_impl);
        return *this;
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

    proxy::operator pw_proxy *() const &
    {
        return get();
    }

    lazy<expected<proxy>> proxy::bind(pw_proxy *raw)
    {
        struct state
        {
            listener hook;
            pw_proxy_events events;
            std::promise<expected<void>> done;
        };

        auto m_state            = std::make_shared<state>();
        m_state->events.version = PW_VERSION_PROXY_EVENTS;

        m_state->events.bound = [](void *data, uint32_t)
        {
            auto &m_state = *reinterpret_cast<state *>(data);
            m_state.done.set_value({});
        };

        m_state->events.error = [](void *data, int seq, int res, const char *message)
        {
            auto &m_state = *reinterpret_cast<state *>(data);
            m_state.done.set_value(tl::make_unexpected<error>({seq, res, message}));
        };

        pw_proxy_add_listener(raw, m_state->hook.get(), &m_state->events, m_state.get());

        return make_lazy<expected<proxy>>(
            [m_state, raw]() -> expected<proxy>
            {
                auto res = m_state->done.get_future().get();

                if (!res.has_value())
                {
                    return tl::make_unexpected(res.error());
                }

                return raw;
            });
    }
} // namespace pipewire