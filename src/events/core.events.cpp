#include "core/core.hpp"
#include "core/events.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct core_listener::impl
    {
        pw_core_events events;
    };

    core_listener::~core_listener() = default;

    core_listener::core_listener(pw_core *core) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = PW_VERSION_CORE_EVENTS;

        m_impl->events.info = [](void *data, const pw_core_info *info) {
            auto &thiz = *reinterpret_cast<core_listener *>(data);
            thiz.m_events.at<core_event::info>().fire(core_info::from(info));
        };

        m_impl->events.done = [](void *data, std::uint32_t id, int seq) {
            auto &thiz = *reinterpret_cast<core_listener *>(data);
            thiz.m_events.at<core_event::done>().fire(id, seq);
        };

        m_impl->events.error = [](void *data, std::uint32_t id, int seq, int res, const char *message) {
            auto &thiz = *reinterpret_cast<core_listener *>(data);
            thiz.m_events.at<core_event::error>().fire(id, error{seq, res, message});
        };

        pw_core_add_listener(core, listener::get(), &m_impl->events, this);
    }

    core_listener::core_listener(core_listener &&core_listener) noexcept
        : listener(std::move(core_listener)), m_impl(std::move(core_listener.m_impl))
    {
    }
} // namespace pipewire
