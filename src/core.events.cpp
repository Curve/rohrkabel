#include "core/core.hpp"
#include "core/events.hpp"
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct core_listener::impl
    {
        events_t events;
        pw_core_events core_events;
    };

    core_listener::~core_listener() = default;

    core_listener::core_listener(const core &core) : m_impl(std::make_unique<impl>())
    {
        m_impl->core_events.version = PW_VERSION_CORE_EVENTS;

        m_impl->core_events.info = [](void *data, const pw_core_info *info) {
            auto &m_impl = *reinterpret_cast<impl *>(data);
            m_impl.events.at<core_event::info>().fire(core_info{info->props, info->id, info->cookie, info->name, info->version, info->user_name, info->host_name});
        };
        m_impl->core_events.done = [](void *data, std::uint32_t id, int seq) {
            auto &m_impl = *reinterpret_cast<impl *>(data);
            m_impl.events.at<core_event::done>().fire(id, seq);
        };
        m_impl->core_events.error = [](void *data, std::uint32_t id, int seq, int res, const char *message) {
            auto &m_impl = *reinterpret_cast<impl *>(data);
            m_impl.events.at<core_event::error>().fire(id, seq, res, message);
        };

        // NOLINTNEXTLINE
        pw_core_add_listener(core.get(), &listener::get(), &m_impl->core_events, m_impl.get());
    }

    core_listener::core_listener(core_listener &&core_listener) noexcept : listener(std::move(core_listener)), m_impl(std::move(core_listener.m_impl)) {}

    template <> void core_listener::on<core_event::info>(events_t::get_t<core_event::info> &&callback)
    {
        m_impl->events.at<core_event::info>().set(std::move(callback));
    }

    template <> void core_listener::on<core_event::done>(events_t::get_t<core_event::done> &&callback)
    {
        m_impl->events.at<core_event::done>().set(std::move(callback));
    }
} // namespace pipewire