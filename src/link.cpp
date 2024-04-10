#include "link/link.hpp"
#include "link/events.hpp"

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

    template <>
    link_listener link::listen()
    {
        return {get()};
    }

    link::operator pw_link *() const &
    {
        return get();
    }

    lazy<expected<link>> link::bind(pw_link *raw)
    {
        struct state
        {
            link_listener listener;

          public:
            std::promise<link_info> info;
        };

        auto proxy = proxy::bind(reinterpret_cast<pw_proxy *>(raw));

        auto m_state    = std::make_shared<state>(raw);
        auto weak_state = std::weak_ptr{m_state};

        m_state->listener.once<link_event::info>([weak_state](link_info info) {
            weak_state.lock()->info.set_value(std::move(info));
        });

        return make_lazy<expected<link>>([m_state, fut = std::move(proxy)]() mutable -> expected<link> {
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
