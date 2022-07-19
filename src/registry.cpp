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

    template <> node registry::bind(std::uint32_t id, update_strategy strategy)
    {
        node rtn{*this, id};

        m_core.update(strategy);

        return rtn;
    }

    template <> port registry::bind(std::uint32_t id, update_strategy strategy)
    {
        port rtn{*this, id};

        m_core.update(strategy);

        return rtn;
    }

    template <> client registry::bind(std::uint32_t id, update_strategy strategy)
    {
        client rtn{*this, id};

        m_core.update(strategy);

        return rtn;
    }

    template <> device registry::bind(std::uint32_t id, update_strategy strategy)
    {
        device rtn{*this, id};

        m_core.update(strategy);

        return rtn;
    }

    template <> metadata registry::bind(std::uint32_t id, update_strategy strategy)
    {
        metadata rtn{*this, id};

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