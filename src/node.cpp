#include "listener.hpp"
#include "spa/param.hpp"
#include "node/node.hpp"

#include <optional>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct node::impl
    {
        pw_node *node;
        node_info info;
    };

    node::~node() = default;

    node::node(node &&node) noexcept : proxy(std::move(node)), m_impl(std::move(node.m_impl)) {}

    node::node(proxy &&_proxy, node_info info) : proxy(std::move(_proxy)), m_impl(std::make_unique<impl>())
    {
        m_impl->info = std::move(info);
        m_impl->node = reinterpret_cast<pw_node *>(proxy::get());
    }

    node &node::operator=(node &&node) noexcept
    {
        proxy::operator=(std::move(node));
        m_impl = std::move(node.m_impl);
        return *this;
    }

    lazy_expected<node> node::bind(pw_node *raw_node)
    {
        struct state
        {
            pw_node_events events;
            std::optional<listener> hook;
            std::promise<node_info> info;
        };

        auto proxy = proxy::bind(reinterpret_cast<pw_proxy *>(raw_node));
        auto m_state = std::make_shared<state>();

        m_state->hook.emplace();
        m_state->events.version = PW_VERSION_NODE_EVENTS;

        m_state->events.info = [](void *data, const pw_node_info *info) {
            auto &m_state = *reinterpret_cast<state *>(data);

            node_info m_info = {info->id,
                                info->max_input_ports,                   //
                                info->max_output_ports,                  //
                                info->n_input_ports,                     //
                                info->n_output_ports,                    //
                                info->props ? info->props : spa::dict{}, //
                                static_cast<node_state>(info->state),    //
                                info->error ? info->error : "",          //
                                info->change_mask,                       //
                                std::vector<param_info>(info->n_params)};

            if (info->params)
            {
                for (auto i = 0u; i < info->n_params; i++)
                {
                    auto param = info->params[i];
                    m_info.params.emplace_back(param_info{param.id, static_cast<param_info_flags>(param.flags)});
                }
            }

            m_state.info.set_value(m_info);
            m_state.hook.reset();
        };

        // NOLINTNEXTLINE
        pw_node_add_listener(raw_node, &m_state->hook->get(), &m_state->events, m_state.get());

        return std::async(std::launch::deferred, [m_state, proxy_fut = std::move(proxy)]() mutable -> tl::expected<node, error> {
            auto proxy = proxy_fut.get();

            if (!proxy.has_value())
            {
                return tl::make_unexpected(proxy.error());
            }

            return node(std::move(proxy.value()), m_state->info.get_future().get());
        });
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