#include "spa/hook.hpp"

#include <pipewire/pipewire.h>

namespace pipewire::spa
{
    struct deleter
    {
        void operator()(spa_hook *hook)
        {
            spa_hook_remove(hook);
            delete hook;
        }
    };

    struct hook::impl
    {
        std::unique_ptr<spa_hook, deleter> hook;
    };

    hook::~hook() = default;

    hook::hook() : m_impl(std::make_unique<impl>())
    {
        m_impl->hook = std::unique_ptr<spa_hook, deleter>(new spa_hook);
        spa_zero(*m_impl->hook);
    }

    hook::hook(hook &&other) noexcept : m_impl(std::move(other.m_impl)) {}

    hook &hook::operator=(hook &&other) noexcept
    {
        m_impl = std::move(other.m_impl);
        return *this;
    }

    spa_hook *hook::get() const
    {
        return m_impl->hook.get();
    }

    hook::operator spa_hook *() const &
    {
        return get();
    }
} // namespace pipewire::spa
