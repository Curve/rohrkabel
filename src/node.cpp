#include "listener.hpp"
#include "spa/param.hpp"
#include "node/node.hpp"
#include "registry/registry.hpp"

#include <optional>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct node::impl
    {
        pw_node *node;
        node_info info;
        pw_node_events events;
        std::optional<listener> hook;
    };

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
                           info->change_mask,
                           std::vector<param_info>(info->n_params)};

            if (info->params)
            {
                for (auto i = 0u; i < info->n_params; i++)
                {
                    auto param = info->params[i];
                    m_impl.info.params.emplace_back(param_info{param.id, static_cast<param_info_flags>(param.flags)});
                }
            }

            m_impl.hook.reset();
        };

        m_impl->hook.emplace();

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

    std::future<std::map<std::uint32_t, spa::pod>> node::params()
    {
        struct state
        {
            listener hook;
            pw_node_events events;
            std::map<std::uint32_t, spa::pod> params;
        };

        auto m_state = std::make_shared<state>();
        m_state->events.version = PW_VERSION_NODE_EVENTS;

        for (const auto &param : m_impl->info.params)
        {
            // NOLINTNEXTLINE
            pw_node_enum_params(m_impl->node, 0, param.id, 0, 1, nullptr);
        }

        m_state->events.param = [](void *data, int, uint32_t id, uint32_t, uint32_t, const struct spa_pod *param) {
            auto &m_state = *reinterpret_cast<state *>(data);
            m_state.params.emplace(id, param);
        };

        // NOLINTNEXTLINE
        pw_node_add_listener(m_impl->node, &m_state->hook.get(), &m_state->events, m_state.get());
        return std::async(std::launch::deferred, [m_state] { return std::map<std::uint32_t, spa::pod>(std::move(m_state->params)); });
    }

    pw_node *node::get() const
    {
        return m_impl->node;
    }

    const std::string node::type = PW_TYPE_INTERFACE_Node;
    const std::uint32_t node::version = PW_VERSION_NODE;
} // namespace pipewire