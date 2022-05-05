#include "listener.hpp"
#include "node/node.hpp"
#include "registry/registry.hpp"

#include <pipewire/pipewire.h>
#include <pipewire/extensions/metadata.h>

namespace pipewire
{
    struct node::impl
    {
        pw_node *node;
        node_info info;
        pw_node_events events;
        std::unique_ptr<listener> hook;
    };

    node::~node()
    {
        if (m_impl)
        {
            pw_proxy_destroy(reinterpret_cast<pw_proxy *>(m_impl->node));
        }
    }

    node::node(node &&node) noexcept : m_impl(std::move(node.m_impl)) {}

    node::node(registry &registry, const global &global) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = PW_VERSION_NODE_EVENTS;

        m_impl->events.info = [](void *data, const pw_node_info *info) {
            auto &m_impl = *reinterpret_cast<impl *>(data);
            m_impl.info = {info->id,
                           info->max_input_ports,
                           info->max_output_ports,
                           info->n_input_ports,
                           info->n_output_ports,
                           info->props ? info->props : dict{},
                           static_cast<node_state>(info->state),
                           {},
                           info->error ? info->error : "",
                           info->change_mask};

            for (auto i = 0u; i < info->n_params; i++)
            {
                auto param = info->params[i];
                m_impl.info.params.emplace_back(param_info{param.id, param.user, param.flags});
            }

            m_impl.hook.reset();
        };

        m_impl->hook = std::make_unique<listener>();
        m_impl->node = reinterpret_cast<pw_node *>(pw_registry_bind(registry.get(), global.id, PW_TYPE_INTERFACE_Node, PW_VERSION_NODE, sizeof(void *)));

        // NOLINTNEXTLINE
        pw_node_add_listener(m_impl->node, &m_impl->hook->get(), &m_impl->events, m_impl.get());
    }

    node &node::operator=(node &&node) noexcept
    {
        m_impl = std::move(node.m_impl);
        return *this;
    }

    node_info node::info() const
    {
        return m_impl->info;
    }

    pw_node *node::get() const
    {
        return m_impl->node;
    }
} // namespace pipewire