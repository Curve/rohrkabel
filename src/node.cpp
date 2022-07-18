#include "listener.hpp"
#include "node/node.hpp"
#include "registry/registry.hpp"

#include <exception>
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

        int last_seq;
        std::map<std::uint32_t, spa::pod> params;
    };

    bool node::is_ready() const
    {
        return !m_impl->hook || !m_impl->params.empty();
    }

    node::~node() = default;

    node::node(pw_node *node) : proxy(reinterpret_cast<pw_proxy *>(node)), m_impl(std::make_unique<impl>())
    {
        m_impl->node = node;
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
                for (auto i = 0u; i < info->n_params; i++)
                {
                    auto param = info->params[i];

                    if (param.flags & SPA_PARAM_INFO_READ)
                    {
                        // NOLINTNEXTLINE
                        m_impl.last_seq = pw_node_enum_params(m_impl.node, 0, param.id, 0, -1, nullptr);
                    }
                }
            }
            else
            {
                m_impl.hook.reset();
            }
        };
        m_impl->events.param = [](void *data, int seq, uint32_t id, [[maybe_unused]] uint32_t index, uint32_t, const struct spa_pod *param) {
            auto &m_impl = *reinterpret_cast<impl *>(data);

            if (seq == m_impl.last_seq)
            {
                m_impl.hook.reset();
            }

            // TODO: Check for duplicate ids.
            m_impl.params.emplace(id, param);
        };

        m_impl->hook = std::make_unique<listener>();

        // NOLINTNEXTLINE
        pw_node_add_listener(m_impl->node, &m_impl->hook->get(), &m_impl->events, m_impl.get());
    }

    node::node(node &&node) noexcept : proxy(std::move(node)), m_impl(std::move(node.m_impl)) {}

    node::node(registry &registry, std::uint32_t id) : node(reinterpret_cast<pw_node *>(pw_registry_bind(registry.get(), id, type.c_str(), version, sizeof(void *)))) {}

    node &node::operator=(node &&node) noexcept
    {
        proxy::proxy::operator=(std::move(node));
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