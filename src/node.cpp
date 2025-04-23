#include "node/node.hpp"
#include "node/events.hpp"

#include "spa/param.hpp"
#include "utils/task.hpp"

#include <pipewire/pipewire.h>
#include <coco/promise/promise.hpp>

namespace pipewire
{
    struct node::impl
    {
        raw_type *node;
        node_info info;
    };

    node::node(proxy &&base, node_info info) : proxy(std::move(base)), m_impl(std::make_unique<impl>())
    {
        m_impl->node = reinterpret_cast<raw_type *>(proxy::get());
        m_impl->info = std::move(info);
    }

    node::node(node &&other) noexcept = default;

    node &node::operator=(node &&other) noexcept = default;

    node::~node() = default;

    void node::set_param(std::uint32_t id, std::uint32_t flags, const spa::pod &pod)
    {
        pw_node_set_param(m_impl->node, id, flags, pod.get());
    }

    lazy<node::params_t> node::params() const
    {
        auto listener = listen();
        auto params   = params_t{};

        listener.on<node_event::param>([&](int, uint32_t id, uint32_t, uint32_t, spa::pod param) {
            params.emplace(id, std::move(param));
        });

        for (const auto &param : m_impl->info.params)
        {
            pw_node_enum_params(m_impl->node, 0, param.id, 0, 1, nullptr);
        }

        co_await lazy<params_t>::wake_on_await{};
        co_return params;
    }

    node::raw_type *node::get() const
    {
        return m_impl->node;
    }

    node_info node::info() const
    {
        return m_impl->info;
    }

    node::operator raw_type *() const &
    {
        return get();
    }

    task<node> node::bind(raw_type *raw)
    {
        auto _proxy   = proxy::bind(reinterpret_cast<proxy::raw_type *>(raw));
        auto listener = node_listener{raw};

        auto promise = coco::promise<node_info>{};
        auto fut     = promise.get_future();

        listener.once<node_event::info>([promise = std::move(promise)](node_info info) mutable {
            promise.set_value(std::move(info));
        });

        auto info  = co_await std::move(fut);
        auto proxy = co_await std::move(_proxy);

        if (!proxy.has_value())
        {
            co_return std::unexpected{proxy.error()};
        }

        co_return node{std::move(proxy.value()), std::move(info)};
    }

    const char *node::type            = PW_TYPE_INTERFACE_Node;
    const std::uint32_t node::version = PW_VERSION_NODE;
} // namespace pipewire
