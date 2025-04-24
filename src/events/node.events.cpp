#include "node/events.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct node_listener::impl
    {
        pw_node_events events;
    };

    node_listener::node_listener(node::raw_type *node) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = version;

        m_impl->events.info = [](void *data, const pw_node_info *info)
        {
            auto &events = *reinterpret_cast<listener::events *>(data);
            events.get<node_event::info>().fire(node_info::from(info));
        };

        m_impl->events.param = [](void *data, int seq, uint32_t id, uint32_t index, uint32_t next, const struct spa_pod *param)
        {
            auto &events = *reinterpret_cast<listener::events *>(data);
            events.get<node_event::param>().fire(seq, id, index, next, spa::pod::copy(param));
        };

        pw_node_add_listener(node, listener::get(), &m_impl->events, m_events.get());
    }

    node_listener::~node_listener() = default;

    const std::uint32_t node_listener::version = PW_VERSION_NODE_EVENTS;
} // namespace pipewire
