#include "core/core.hpp"
#include "core/events.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct core_listener::impl
    {
        pw_core_events events;
    };

    core_listener::core_listener(core::raw_type *core) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = version;

        m_impl->events.info = [](void *data, const pw_core_info *info) {
            auto &events = *reinterpret_cast<listener::events *>(data);
            events.get<core_event::info>().fire(core_info::from(info));
        };

        m_impl->events.done = [](void *data, std::uint32_t id, int seq) {
            auto &events = *reinterpret_cast<listener::events *>(data);
            events.get<core_event::done>().fire(id, seq);
        };

        m_impl->events.error = [](void *data, std::uint32_t id, int seq, int res, const char *message) {
            auto &events = *reinterpret_cast<listener::events *>(data);
            events.get<core_event::error>().fire(id, error{seq, res, message});
        };

        pw_core_add_listener(core, listener::get(), &m_impl->events, m_events.get());
    }

    core_listener::~core_listener() = default;

    const std::uint32_t core_listener::version = PW_VERSION_CORE_EVENTS;
} // namespace pipewire
