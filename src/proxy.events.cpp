#include "proxy/proxy.hpp"
#include "proxy/events.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct proxy_listener::impl
    {
        pw_proxy_events events;
    };

    proxy_listener::~proxy_listener() = default;

    proxy_listener::proxy_listener(pw_proxy *proxy) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = PW_VERSION_PROXY_EVENTS;

        m_impl->events.error = [](void *data, int seq, int res, const char *message) {
            auto &thiz = *reinterpret_cast<proxy_listener *>(data);
            thiz.m_events.at<proxy_event::error>().fire(seq, res, message);
        };

        m_impl->events.bound = [](void *data, std::uint32_t global) {
            auto &thiz = *reinterpret_cast<proxy_listener *>(data);
            thiz.m_events.at<proxy_event::bound>().fire(global);
        };

        m_impl->events.bound_props = [](void *data, std::uint32_t global, const spa_dict *props) {
            auto &thiz = *reinterpret_cast<proxy_listener *>(data);
            thiz.m_events.at<proxy_event::bound_props>().fire(global, props);
        };

        pw_proxy_add_listener(proxy, listener::get(), &m_impl->events, this);
    }

    proxy_listener::proxy_listener(proxy_listener &&proxy_listener) noexcept
        : listener(std::move(proxy_listener)), m_impl(std::move(proxy_listener.m_impl))
    {
    }

    void proxy_listener::clear(proxy_event event)
    {
        m_events.clear(event);
    }

    void proxy_listener::remove(proxy_event event, std::uint64_t id)
    {
        m_events.remove(event, id);
    }

    template <>
    std::uint64_t proxy_listener::on<proxy_event::error>(events::type_t<proxy_event::error> &&callback)
    {
        return m_events.at<proxy_event::error>().add(std::move(callback));
    }

    template <>
    std::uint64_t proxy_listener::on<proxy_event::bound>(events::type_t<proxy_event::bound> &&callback)
    {
        return m_events.at<proxy_event::bound>().add(std::move(callback));
    }

    template <>
    std::uint64_t proxy_listener::on<proxy_event::bound_props>(events::type_t<proxy_event::bound_props> &&callback)
    {
        return m_events.at<proxy_event::bound_props>().add(std::move(callback));
    }
} // namespace pipewire
