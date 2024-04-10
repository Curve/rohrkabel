#include "registry/events.hpp"
#include "registry/registry.hpp"

#include "utils/check.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct registry::impl
    {
        pw_registry *registry;
        std::shared_ptr<pipewire::core> core;
    };

    void *registry::bind(std::uint32_t id, const char *type, std::uint32_t version) const
    {
        return pw_registry_bind(get(), id, type, version, 0);
    }

    registry::~registry()
    {
        pw_proxy_destroy(reinterpret_cast<pw_proxy *>(m_impl->registry));
    }

    registry::registry() : m_impl(std::make_unique<impl>()) {}

    template <>
    registry_listener registry::listen()
    {
        return {get()};
    }

    pw_registry *registry::get() const
    {
        return m_impl->registry;
    }

    std::shared_ptr<core> registry::core() const
    {
        return m_impl->core;
    }

    registry::operator pw_registry *() const &
    {
        return get();
    }

    std::shared_ptr<registry> registry::create(std::shared_ptr<pipewire::core> core)
    {
        auto *registry = pw_core_get_registry(core->get(), PW_VERSION_REGISTRY, 0);
        check(registry, "Failed to get registry");

        if (!registry)
        {
            return nullptr;
        }

        auto rtn = std::unique_ptr<pipewire::registry>(new pipewire::registry);

        rtn->m_impl->registry = registry;
        rtn->m_impl->core     = std::move(core);

        return rtn;
    }
} // namespace pipewire
