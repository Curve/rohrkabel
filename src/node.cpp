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

        std::uint32_t last_id;
        std::map<std::uint32_t, spa::pod> params;
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
                           info->props ? info->props : spa::dict{},
                           static_cast<node_state>(info->state),
                           info->error ? info->error : "",
                           info->change_mask};

            if (info->params)
            {
                m_impl.last_id = info->params[info->n_params - 1].id;

                for (auto i = 0u; i < info->n_params; i++)
                {
                    auto param = info->params[i];

                    if (param.flags & SPA_PARAM_INFO_READ)
                    {
                        // NOLINTNEXTLINE
                        pw_node_enum_params(m_impl.node, 0, param.id, 0, -1, nullptr);
                    }
                }
            }
        };
        m_impl->events.param = [](void *data, int, uint32_t id, uint32_t, uint32_t, const struct spa_pod *param) {
            auto &m_impl = *reinterpret_cast<impl *>(data);
            m_impl.params.emplace(id, param);

            if (id == m_impl.last_id)
            {
                m_impl.hook.reset();
            }
        };

        m_impl->hook = std::make_unique<listener>();
        m_impl->node = reinterpret_cast<pw_node *>(pw_registry_bind(registry.get(), global.id, type.c_str(), version, sizeof(void *)));

        // NOLINTNEXTLINE
        pw_node_add_listener(m_impl->node, &m_impl->hook->get(), &m_impl->events, m_impl.get());
    }

    node &node::operator=(node &&node) noexcept
    {
        m_impl = std::move(node.m_impl);
        return *this;
    }

    void node::set_param(std::uint32_t id, const spa::pod &pod)
    {
        // NOLINTNEXTLINE
        pw_node_set_param(m_impl->node, id, 0, pod.get());
    }

    node_info node::info() const
    {
        return m_impl->info;
    }

    const std::map<std::uint32_t, spa::pod> &node::params() const
    {
        return m_impl->params;
    }

    pw_node *node::get() const
    {
        return m_impl->node;
    }

    const std::string node::type = PW_TYPE_INTERFACE_Node;
    const std::uint32_t node::version = PW_VERSION_NODE;
} // namespace pipewire