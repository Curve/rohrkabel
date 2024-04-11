#include "link/link.hpp"
#include "link/events.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct link_listener::impl
    {
        pw_link_events events;
    };

    link_listener::~link_listener() = default;

    link_listener::link_listener(link::raw_type *link) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = PW_VERSION_LINK_EVENTS;

        m_impl->events.info = [](void *data, const pw_link_info *info) {
            auto &thiz = *reinterpret_cast<link_listener *>(data);
            thiz.m_events.at<link_event::info>().fire(link_info::from(info));
        };

        pw_link_add_listener(link, listener::get(), &m_impl->events, this);
    }

    link_listener::link_listener(link_listener &&link_listener) noexcept
        : listener(std::move(link_listener)), m_impl(std::move(link_listener.m_impl))
    {
    }
} // namespace pipewire
