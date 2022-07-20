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
        std::uint32_t id;
        pw_proxy_events events;
        std::optional<listener> hook;
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
        m_impl->events.version = PW_VERSION_PROXY_EVENTS;

        m_impl->events.bound = [](void *data, std::uint32_t id) {
            auto &m_impl = *reinterpret_cast<impl *>(data);

            m_impl.id = id;
            m_impl.hook.reset();
        };
        m_impl->events.error = []([[maybe_unused]] void *data, int seq, int res, const char *message) {
            auto &m_impl = *reinterpret_cast<impl *>(data);

            m_impl.hook.reset();
            throw error(seq, res, message);
        };

        m_impl->hook.emplace();
        pw_proxy_add_listener(m_impl->proxy, &m_impl->hook->get(), &m_impl->events, m_impl.get());
    }

    proxy &proxy::operator=(proxy &&proxy) noexcept
    {
        m_impl = std::move(proxy.m_impl);
        return *this;
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