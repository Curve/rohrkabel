#include <pipewire/pipewire.h>
#include "registry/registry.hpp"

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
    }

    template <> registry_listener registry::listen<registry_listener>()
    {
        return {*this};
    }

    template <> node registry::bind<node>(const global &global, bool auto_sync)
    {
        node rtn{*this, global};

        if (auto_sync)
        {
            m_core.sync();
        }

        return rtn;
    }

    template <> port registry::bind<port>(const global &global, bool auto_sync)
    {
        port rtn{*this, global};

        if (auto_sync)
        {
            m_core.sync();
        }

        return rtn;
    }

    template <> device registry::bind<device>(const global &global, bool auto_sync)
    {
        device rtn{*this, global};

        if (auto_sync)
        {
            m_core.sync();
        }

        return rtn;
    }

    template <> metadata registry::bind<metadata>(const global &global, bool auto_sync)
    {
        metadata rtn{*this, global};

        if (auto_sync)
        {
            m_core.sync();
        }

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