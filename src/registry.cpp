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

    template <> registry_listener registry::listen<registry_listener>()
    {
        return {*this};
    }

    template <> node registry::bind<node>(std::uint32_t id, bool auto_sync)
    {
        node rtn{*this, id};

        if (auto_sync)
        {
            m_core.sync(2ul); //? We need to sync twice for the param binding
        }

        return rtn;
    }

    template <> port registry::bind<port>(std::uint32_t id, bool auto_sync)
    {
        port rtn{*this, id};

        if (auto_sync)
        {
            m_core.sync(2ul); //? We need to sync twice for the param binding
        }

        return rtn;
    }

    template <> client registry::bind<client>(std::uint32_t id, bool auto_sync)
    {
        client rtn{*this, id};

        if (auto_sync)
        {
            m_core.sync();
        }

        return rtn;
    }

    template <> device registry::bind<device>(std::uint32_t id, bool auto_sync)
    {
        device rtn{*this, id};

        if (auto_sync)
        {
            m_core.sync(2ul); //? We need to sync twice for the param binding
        }

        return rtn;
    }

    template <> metadata registry::bind<metadata>(std::uint32_t id, bool auto_sync)
    {
        metadata rtn{*this, id};

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