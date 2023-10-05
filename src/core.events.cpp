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

    core_listener::core_listener(const core &core) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = PW_VERSION_CORE_EVENTS;

        m_impl->events.info = [](void *data, const pw_core_info *info)
        {
            auto &thiz = *reinterpret_cast<core_listener *>(data);

            thiz.m_events.at<core_event::info>().fire(core_info{
                info->props,
                info->id,
                info->cookie,
                info->name,
                info->version,
                info->user_name,
                info->host_name,
            });
        };

        m_impl->events.done = [](void *data, std::uint32_t id, int seq)
        {
            auto &thiz = *reinterpret_cast<core_listener *>(data);
            thiz.m_events.at<core_event::done>().fire(id, seq);
        };

        m_impl->events.error = [](void *data, std::uint32_t id, int seq, int res, const char *message)
        {
            auto &thiz = *reinterpret_cast<core_listener *>(data);
            thiz.m_events.at<core_event::error>().fire(id, error{seq, res, message});
        };

        pw_core_add_listener(core.get(), listener::get(), &m_impl->events, this);
    }

    core_listener::core_listener(core_listener &&core_listener) noexcept
        : listener(std::move(core_listener)), m_impl(std::move(core_listener.m_impl))
    {
    }

    void core_listener::clear(core_event event)
    {
        m_events.clear(event);
    }

    void core_listener::remove(core_event event, std::uint64_t id)
    {
        m_events.remove(event, id);
    }

    template <>
    std::uint64_t core_listener::on<core_event::info>(events::type_t<core_event::info> &&callback)
    {
        return m_events.at<core_event::info>().add(std::move(callback));
    }

    template <>
    std::uint64_t core_listener::on<core_event::done>(events::type_t<core_event::done> &&callback)
    {
        return m_events.at<core_event::done>().add(std::move(callback));
    }

    template <>
    std::uint64_t core_listener::on<core_event::error>(events::type_t<core_event::error> &&callback)
    {
        return m_events.at<core_event::error>().add(std::move(callback));
    }
} // namespace pipewire