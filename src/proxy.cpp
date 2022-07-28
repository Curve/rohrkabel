#include "proxy.hpp"
#include "error.hpp"
#include "listener.hpp"

#include <optional>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct proxy::impl
    {
        pw_proxy *proxy;
    };

    proxy::~proxy()
    {
        if (m_impl)
        {
            pw_proxy_destroy(m_impl->proxy);
        }
    }

    proxy::proxy(proxy &&proxy) noexcept : m_impl(std::move(proxy.m_impl)) {}

    proxy::proxy(pw_proxy *proxy) : m_impl(std::make_unique<impl>())
    {
        m_impl->proxy = proxy;
    }

    proxy &proxy::operator=(proxy &&proxy) noexcept
    {
        m_impl = std::move(proxy.m_impl);
        return *this;
    }

    lazy_expected<proxy> proxy::bind(pw_proxy *raw_proxy)
    {
        struct state
        {
            listener hook;
            pw_proxy_events events;
            std::promise<void> done;
            std::optional<error> error;
        };

        auto m_state = std::make_shared<state>();
        m_state->events.version = PW_VERSION_PROXY_EVENTS;

        m_state->events.bound = [](void *data, uint32_t) {
            auto &m_state = *reinterpret_cast<state *>(data);
            m_state.done.set_value();
        };

        m_state->events.error = [](void *data, int seq, int res, const char *message) {
            auto &m_state = *reinterpret_cast<state *>(data);
            m_state.error.emplace(seq, res, message);
            m_state.done.set_value();
        };

        pw_proxy_add_listener(raw_proxy, &m_state->hook.get(), &m_state->events, m_state.get());

        return std::async(std::launch::deferred, [m_state, raw_proxy]() -> tl::expected<proxy, error> {
            m_state->done.get_future().wait();

            if (m_state->error)
            {
                return tl::make_unexpected(*m_state->error);
            }

            return raw_proxy;
        });
    }

    std::uint32_t proxy::id() const
    {
        return pw_proxy_get_bound_id(m_impl->proxy);
    }

    std::uint32_t proxy::id() const
    {
        return m_impl->id;
    }

    pw_proxy *proxy::get() const
    {
        return m_impl->proxy;
    }

    std::uint32_t proxy::release()
    {
        auto rtn = m_impl->id;
        m_impl.reset();
        return rtn;
    }
} // namespace pipewire