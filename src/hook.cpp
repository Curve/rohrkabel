#include "spa/hook.hpp"

#include <pipewire/pipewire.h>

namespace pipewire::spa
{
    struct hook::impl
    {
        pw_unique_ptr<raw_type> hook;
    };

    hook::~hook() = default;

    hook::hook(deleter<raw_type> deleter, raw_type *raw)
        : m_impl(std::make_unique<impl>(pw_unique_ptr<raw_type>{raw, deleter}))
    {
    }

    hook::hook(hook &&other) noexcept : m_impl(std::move(other.m_impl)) {}

    hook &hook::operator=(hook &&other) noexcept
    {
        m_impl = std::move(other.m_impl);
        return *this;
    }

    hook::raw_type *hook::get() const
    {
        return m_impl->hook.get();
    }

    hook hook::create()
    {
        auto *hook = new raw_type;
        spa_zero(*hook);

        return from(hook);
    }

    hook hook::from(raw_type *hook)
    {
        static constexpr auto deleter = [](auto *hook) {
            spa_hook_remove(hook);
            delete hook;
        };

        return {deleter, hook};
    }

    hook hook::view(raw_type *hook)
    {
        return {view_deleter<raw_type>, hook};
    }

    hook::operator raw_type *() const &
    {
        return get();
    }
} // namespace pipewire::spa
