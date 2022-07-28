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

    link::link(link &&link) noexcept : proxy(std::move(link)), m_impl(std::move(link.m_impl)) {}

    link::link(proxy &&_proxy, link_info info) : proxy(std::move(_proxy)), m_impl(std::make_unique<impl>())
    {
        m_impl->info = std::move(info);
        m_impl->link = reinterpret_cast<pw_link *>(proxy::get());
    }

    link &link::operator=(link &&link) noexcept
    {
        proxy::operator=(std::move(link));
        m_impl = std::move(link.m_impl);
        return *this;
    }

    lazy_expected<link> link::bind(pw_link *raw_link)
    {
        struct state
        {
            pw_link_events events;
            std::optional<listener> hook;
            std::promise<link_info> info;
        };

        auto proxy = proxy::bind(reinterpret_cast<pw_proxy *>(raw_link));
        auto m_state = std::make_shared<state>();

        m_state->hook.emplace();
        m_state->events.version = PW_VERSION_LINK_EVENTS;

        m_state->events.info = [](void *data, const pw_link_info *info) {
            auto &m_state = *reinterpret_cast<state *>(data);
            m_state.info.set_value({info->id,
                                    info->input_port_id,                     //
                                    info->input_node_id,                     //
                                    info->output_port_id,                    //
                                    info->output_node_id,                    //
                                    info->change_mask,                       //
                                    info->props ? info->props : spa::dict{}, //
                                    static_cast<link_state>(info->state)});
            m_state.hook.reset();
        };

        // NOLINTNEXTLINE
        pw_link_add_listener(raw_link, &m_state->hook->get(), &m_state->events, m_state.get());

        return std::async(std::launch::deferred, [m_state, proxy_fut = std::move(proxy)]() mutable -> tl::expected<link, error> {
            auto proxy = proxy_fut.get();

            if (!proxy.has_value())
            {
                return tl::make_unexpected(proxy.error());
            }

            return link(std::move(proxy.value()), m_state->info.get_future().get());
        });
    }

    link_info link::info() const
    {
        return m_impl->info;
    }

    pw_link *link::get() const
    {
        return m_impl->link;
    }

    const std::string link::type = PW_TYPE_INTERFACE_Link;
    const std::uint32_t link::version = PW_VERSION_LINK;
} // namespace pipewire