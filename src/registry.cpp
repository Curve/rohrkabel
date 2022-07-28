#include "registry/registry.hpp"

#include <cassert>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct registry::impl
    {
        pw_registry *registry;
    };

    registry::~registry()
    {
        pw_proxy_destroy(reinterpret_cast<pw_proxy *>(m_impl->registry));
    }

    registry::registry(core &core) : m_core(core), m_impl(std::make_unique<impl>())
    {
        m_impl->registry = pw_core_get_registry(core.get(), PW_VERSION_REGISTRY, 0);
        assert((void("Failed to get registry"), m_impl->registry));
    }

    template <> registry_listener registry::listen()
    {
        return {*this};
    }

    template <> lazy_expected<node> registry::bind(std::uint32_t id, update_strategy strategy)
    {
        auto rtn = node::bind(reinterpret_cast<pw_node *>(pw_registry_bind(get(), id, node::type.c_str(), node::version, sizeof(void *))));
        m_core.update(strategy);
        return rtn;
    }

    template <> lazy_expected<port> registry::bind(std::uint32_t id, update_strategy strategy)
    {
        auto rtn = port::bind(reinterpret_cast<pw_port *>(pw_registry_bind(get(), id, port::type.c_str(), port::version, sizeof(void *))));
        m_core.update(strategy);
        return rtn;
    }

    template <> lazy_expected<client> registry::bind(std::uint32_t id, update_strategy strategy)
    {
        auto rtn = client::bind(reinterpret_cast<pw_client *>(pw_registry_bind(get(), id, client::type.c_str(), client::version, sizeof(void *))));
        m_core.update(strategy);
        return rtn;
    }

    template <> lazy_expected<device> registry::bind(std::uint32_t id, update_strategy strategy)
    {
        auto rtn = device::bind(reinterpret_cast<pw_device *>(pw_registry_bind(get(), id, device::type.c_str(), device::version, sizeof(void *))));
        m_core.update(strategy);
        return rtn;
    }

    template <> lazy_expected<metadata> registry::bind(std::uint32_t id, update_strategy strategy)
    {
        auto rtn = metadata::bind(reinterpret_cast<pw_metadata *>(pw_registry_bind(get(), id, metadata::type.c_str(), metadata::version, sizeof(void *))));
        m_core.update(strategy);
        return rtn;
    }

    core &registry::get_core()
    {
        return m_core;
    }

    pw_registry *registry::get() const
    {
        return m_impl->registry;
    }
} // namespace pipewire