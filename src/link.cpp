#include "listener.hpp"
#include "link/link.hpp"

#include <optional>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct link::impl
    {
        pw_link *link;
        link_info info;
    };

    link::~link() = default;

    link::link(link &&other) noexcept : proxy(std::move(other)), m_impl(std::move(other.m_impl)) {}

    link::link(proxy &&base, link_info info) : proxy(std::move(base)), m_impl(std::make_unique<impl>())
    {
        m_impl->link = reinterpret_cast<pw_link *>(proxy::get());
        m_impl->info = std::move(info);
    }

    link &link::operator=(link &&other) noexcept
    {
        proxy::operator=(std::move(other));
        m_impl = std::move(other.m_impl);
        return *this;
    }

    pw_link *link::get() const
    {
        return m_impl->link;
    }

    link_info link::info() const
    {
        return m_impl->info;
    }

    link::operator pw_link *() const &
    {
        return get();
    }

    lazy<expected<link>> link::bind(pw_link *raw)
    {
        struct state
        {
            pw_link_events events;
            std::optional<listener> hook;
            std::promise<link_info> info;
        };

        auto proxy   = proxy::bind(reinterpret_cast<pw_proxy *>(raw));
        auto m_state = std::make_shared<state>();

        m_state->hook.emplace();
        m_state->events.version = PW_VERSION_LINK_EVENTS;

        m_state->events.info = [](void *data, const pw_link_info *info)
        {
            auto &m_state = *reinterpret_cast<state *>(data);

            m_state.info.set_value({
                info->props,
                static_cast<link_state>(info->state),
                info->id,
                info->change_mask,
                {
                    .port = info->input_port_id,
                    .node = info->input_node_id,
                },
                {
                    .port = info->output_port_id,
                    .node = info->output_node_id,
                },
            });

            m_state.hook.reset();
        };

        // NOLINTNEXTLINE(*-optional-access)
        pw_link_add_listener(raw, m_state->hook->get(), &m_state->events, m_state.get());

        return make_lazy<expected<link>>(
            [m_state, fut = std::move(proxy)]() mutable -> expected<link>
            {
                auto proxy = fut.get();

                if (!proxy.has_value())
                {
                    return tl::make_unexpected(proxy.error());
                }

                return link{std::move(proxy.value()), m_state->info.get_future().get()};
            });
    }

    const char *link::type            = PW_TYPE_INTERFACE_Link;
    const std::uint32_t link::version = PW_VERSION_LINK;
} // namespace pipewire
