#include "registry/events.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct registry_listener::impl
    {
        pw_registry_events events;
    };

    registry_listener::registry_listener(registry::raw_type *registry) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = version;

        m_impl->events.global = [](void *data, std::uint32_t id, std::uint32_t permissions, const char *type,
                                   std::uint32_t version, const spa_dict *props)
        {
            auto &events = *reinterpret_cast<listener::events *>(data);
            events.get<registry_event::global>().fire(global{
                .id          = id,
                .version     = version,
                .permissions = permissions,
                .props       = props,
                .type        = type ? type : "",
            });
        };

        m_impl->events.global_remove = [](void *data, std::uint32_t id)
        {
            auto &events = *reinterpret_cast<listener::events *>(data);
            events.get<registry_event::global_removed>().fire(id);
        };

        pw_registry_add_listener(registry, listener::get(), &m_impl->events, m_events.get());
    }

    registry_listener::~registry_listener() = default;

    const std::uint32_t registry_listener::version = PW_VERSION_REGISTRY_EVENTS;
} // namespace pipewire
