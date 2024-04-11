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

    proxy_listener::proxy_listener(proxy::raw_type *proxy) : m_impl(std::make_unique<impl>())
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
} // namespace pipewire
