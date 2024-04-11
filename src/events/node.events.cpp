#include "node/node.hpp"
#include "node/events.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct node_listener::impl
    {
        pw_node_events events;
    };

    node_listener::~node_listener() = default;

    node_listener::node_listener(node::raw_type *node) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = PW_VERSION_NODE_EVENTS;

        m_impl->events.info = [](void *data, const pw_node_info *info) {
            auto &thiz = *reinterpret_cast<node_listener *>(data);
            thiz.m_events.at<node_event::info>().fire(node_info::from(info));
        };

        m_impl->events.param = [](void *data, int seq, uint32_t id, uint32_t index, uint32_t next,
                                  const struct spa_pod *param) {
            auto &thiz = *reinterpret_cast<node_listener *>(data);
            thiz.m_events.at<node_event::param>().fire(seq, id, index, next, spa::pod::copy(param));
        };

        pw_node_add_listener(node, listener::get(), &m_impl->events, this);
    }

    node_listener::node_listener(node_listener &&node_listener) noexcept
        : listener(std::move(node_listener)), m_impl(std::move(node_listener.m_impl))
    {
    }
} // namespace pipewire
