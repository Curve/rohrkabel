#include "spa/hook.hpp"

#include <pipewire/pipewire.h>

namespace pipewire::spa
{
    struct deleter
    {
        void operator()(hook::raw_type *hook)
        {
            spa_hook_remove(hook);
            delete hook;
        }
    };

    using unique_ptr = std::unique_ptr<hook::raw_type, deleter>;

    struct hook::impl
    {
        unique_ptr hook;
    };

    hook::~hook() = default;

    hook::hook() : m_impl(std::make_unique<impl>())
    {
        m_impl->hook = unique_ptr(new raw_type);
        spa_zero(*m_impl->hook);
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

    hook::operator raw_type *() const &
    {
        return get();
    }
} // namespace pipewire::spa
