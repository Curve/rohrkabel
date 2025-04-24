#include "registry/registry.hpp"

#include "proxy/proxy.hpp"
#include "utils/check.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct registry::impl
    {
        pw_unique_ptr<raw_type> registry;
        std::shared_ptr<pipewire::core> core;
    };

    void *registry::bind(std::uint32_t id, const char *type, std::uint32_t version) const
    {
        return pw_registry_bind(get(), id, type, version, 0);
    }

    registry::registry(deleter<raw_type> deleter, raw_type *raw, std::shared_ptr<pipewire::core> core)
        : m_impl(std::make_unique<impl>(pw_unique_ptr<raw_type>{raw, deleter}, std::move(core)))
    {
    }

    registry::registry(registry &&) noexcept = default;

    registry &registry::operator=(registry &&) noexcept = default;

    registry::~registry() = default;

    registry::raw_type *registry::get() const
    {
        return m_impl->registry.get();
    }

    std::shared_ptr<core> registry::core() const
    {
        return m_impl->core;
    }

    registry::operator raw_type *() const &
    {
        return get();
    }

    std::optional<registry> registry::create(std::shared_ptr<pipewire::core> core)
    {
        auto *registry = pw_core_get_registry(core->get(), version, 0);
        check(registry, "Failed to get registry");

        if (!registry)
        {
            return std::nullopt;
        }

        return from(registry, std::move(core));
    }

    registry registry::from(raw_type *registry, std::shared_ptr<pipewire::core> core)
    {
        static constexpr auto deleter = [](auto *registry)
        {
            pw_proxy_destroy(reinterpret_cast<proxy::raw_type *>(registry));
        };

        return {deleter, registry, std::move(core)};
    }

    registry registry::view(raw_type *registry, std::shared_ptr<pipewire::core> core)
    {
        return {view_deleter<raw_type>, registry, std::move(core)};
    }

    const char *registry::type            = PW_TYPE_INTERFACE_Registry;
    const std::uint32_t registry::version = PW_VERSION_REGISTRY;
} // namespace pipewire
