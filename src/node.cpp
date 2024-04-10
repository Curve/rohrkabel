#include "node/node.hpp"
#include "node/events.hpp"

#include "spa/param.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct node::impl
    {
        pw_node *node;
        node_info info;
    };

    node::~node() = default;

    node::node(node &&other) noexcept : proxy(std::move(other)), m_impl(std::move(other.m_impl)) {}

    node::node(proxy &&base, node_info info) : proxy(std::move(base)), m_impl(std::make_unique<impl>())
    {
        m_impl->node = reinterpret_cast<pw_node *>(proxy::get());
        m_impl->info = std::move(info);
    }

    node &node::operator=(node &&other) noexcept
    {
        proxy::operator=(std::move(other));
        m_impl = std::move(other.m_impl);
        return *this;
    }

    void node::set_param(std::uint32_t id, std::uint32_t flags, const spa::pod &pod)
    {
        pw_node_set_param(m_impl->node, id, flags, pod.get());
    }

    lazy<node::params_t> node::params()
    {
        struct state
        {
            node_listener listener;

          public:
            params_t params;
        };

        auto m_state    = std::make_shared<state>(get());
        auto weak_state = std::weak_ptr{m_state};

        m_state->listener.on<node_event::param>([weak_state](int, uint32_t id, uint32_t, uint32_t, spa::pod param) {
            weak_state.lock()->params.emplace(id, std::move(param));
        });

        for (const auto &param : m_impl->info.params)
        {
            pw_node_enum_params(m_impl->node, 0, param.id, 0, 1, nullptr);
        }

        return make_lazy<params_t>([m_state]() -> params_t {
            return m_state->params;
        });
    }

    pw_node *node::get() const
    {
        return m_impl->node;
    }

    node_info node::info() const
    {
        return m_impl->info;
    }

    template <>
    node_listener node::listen()
    {
        return {get()};
    }

    node::operator pw_node *() const &
    {
        return get();
    }

    lazy<expected<node>> node::bind(pw_node *raw)
    {
        struct state
        {
            node_listener listener;

          public:
            std::promise<node_info> info;
        };

        auto proxy = proxy::bind(reinterpret_cast<pw_proxy *>(raw));

        auto m_state    = std::make_shared<state>(raw);
        auto weak_state = std::weak_ptr{m_state};

        m_state->listener.once<node_event::info>([weak_state](node_info info) {
            weak_state.lock()->info.set_value(std::move(info));
        });

        return make_lazy<expected<node>>([m_state, proxy_fut = std::move(proxy)]() mutable -> expected<node> {
            auto proxy = proxy_fut.get();

            if (!proxy.has_value())
            {
                return tl::make_unexpected(proxy.error());
            }

            return node{std::move(proxy.value()), m_state->info.get_future().get()};
        });
    }

    const char *node::type            = PW_TYPE_INTERFACE_Node;
    const std::uint32_t node::version = PW_VERSION_NODE;
} // namespace pipewire
