#include "link/link.hpp"
#include "link/events.hpp"

#include <pipewire/pipewire.h>
#include <coco/promise/promise.hpp>

namespace pipewire
{
    struct link::impl
    {
        raw_type *link;
        link_info info;
    };

    link::link(proxy &&base, link_info info) : proxy(std::move(base)), m_impl(std::make_unique<impl>())
    {
        m_impl->link = reinterpret_cast<raw_type *>(proxy::get());
        m_impl->info = std::move(info);
    }

    link::link(link &&) noexcept = default;

    link &link::operator=(link &&) noexcept = default;

    link::~link() = default;

    link_info link::info() const
    {
        return m_impl->info;
    }

    link::raw_type *link::get() const
    {
        return m_impl->link;
    }

    link::raw_type *link::release() &&
    {
        std::ignore = std::move(*this).proxy::release();
        return std::exchange(m_impl->link, nullptr);
    }

    link::operator raw_type *() const &
    {
        return get();
    }

    task<link> link::bind(raw_type *raw)
    {
        auto _proxy   = proxy::bind(reinterpret_cast<proxy::raw_type *>(raw));
        auto listener = link_listener{raw};

        auto promise = coco::promise<link_info>{};
        auto fut     = promise.get_future();

        listener.once<link_event::info>(
            [promise = std::move(promise)](link_info info) mutable
            {
                promise.set_value(std::move(info));
            });

        co_await task<link>::make_lazy{};
        auto proxy = co_await std::move(_proxy);

        if (!proxy.has_value())
        {
            co_return std::unexpected{proxy.error()};
        }

        co_return link{std::move(proxy.value()), co_await std::move(fut)};
    }

    const char *link::type            = PW_TYPE_INTERFACE_Link;
    const std::uint32_t link::version = PW_VERSION_LINK;
} // namespace pipewire
