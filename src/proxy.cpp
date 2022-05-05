#include "proxy.hpp"
#include "error.hpp"
#include "listener.hpp"
#include "core/core.hpp"

#include <optional>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct proxy::impl
    {
        pw_proxy *proxy;
        std::uint32_t id;
        pw_proxy_events events;
    };

    proxy::~proxy()
    {
        if (m_impl)
        {
            pw_proxy_destroy(m_impl->proxy);
        }
    }

    proxy::proxy(proxy &&proxy) noexcept : m_impl(std::move(proxy.m_impl)) {}

    proxy::proxy(core &core, const std::string &factory_name, const properties &properties, const std::string &type, std::uint32_t version) : m_impl(std::make_unique<impl>())
    {
        listener hook;
        m_impl->events.version = PW_VERSION_PROXY_EVENTS;

        m_impl->events.bound = [](void *data, std::uint32_t id) {
            auto &m_impl = *reinterpret_cast<impl *>(data);
            m_impl.id = id;
        };
        m_impl->events.error = []([[maybe_unused]] void *data, int seq, int res, const char *message) {
            //
            throw error(seq, res, message);
        };

        m_impl->proxy = reinterpret_cast<pw_proxy *>(pw_core_create_object(core.get(), factory_name.c_str(), type.c_str(), version, &properties.get()->dict, sizeof(void *)));

        pw_proxy_add_listener(m_impl->proxy, &hook.get(), &m_impl->events, m_impl.get());
        core.sync();
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
} // namespace pipewire