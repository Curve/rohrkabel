#include "registry/events.hpp"
#include "registry/registry.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct registry_listener::impl
    {
        events_t events;
        pw_registry_events registry_events;
    };

    registry_listener::~registry_listener() = default;

    registry_listener::registry_listener(const registry &registry) : m_impl(std::make_unique<impl>())
    {
        m_impl->registry_events.version = PW_VERSION_REGISTRY_EVENTS;

        m_impl->registry_events.global = [](void *data, std::uint32_t id, std::uint32_t permissions, const char *type, std::uint32_t version, const spa_dict *props) {
            auto &m_impl = *reinterpret_cast<impl *>(data);
            m_impl.events.at<registry_event::global>().fire(global{id, version, permissions, props, type});
        };
        m_impl->registry_events.global_remove = [](void *data, std::uint32_t id) {
            auto &m_impl = *reinterpret_cast<impl *>(data);
            m_impl.events.at<registry_event::global_removed>().fire(id);
        };

        // NOLINTNEXTLINE
        pw_registry_add_listener(registry.get(), &listener::get(), &m_impl->registry_events, m_impl.get());
    }

    registry_listener::registry_listener(registry_listener &&registry_listener) noexcept : listener(std::move(registry_listener)), m_impl(std::move(registry_listener.m_impl)) {}

    template <> void registry_listener::on<registry_event::global>(events_t::get_t<registry_event::global> &&callback)
    {
        m_impl->events.at<registry_event::global>().set(std::move(callback));
    }

    template <> void registry_listener::on<registry_event::global_removed>(events_t::get_t<registry_event::global_removed> &&callback)
    {
        m_impl->events.at<registry_event::global_removed>().set(std::move(callback));
    }
} // namespace pipewire