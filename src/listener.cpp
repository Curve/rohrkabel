#include "listener.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct deleter
    {
        void operator()(spa_hook *hook)
        {
            spa_hook_remove(hook);
            delete hook;
        }
    };

    struct listener::impl
    {
        std::unique_ptr<spa_hook, deleter> hook;
    };

    listener::~listener() = default;

    listener::listener() : m_impl(std::make_unique<impl>())
    {
        m_impl->hook = std::unique_ptr<spa_hook, deleter>(new spa_hook);
        spa_zero(*m_impl->hook);
    }

    listener::listener(listener &&other) noexcept : m_impl(std::move(other.m_impl)) {}

    listener &listener::operator=(listener &&other) noexcept
    {
        m_impl = std::move(other.m_impl);
        return *this;
    }

    spa_hook *listener::get() const
    {
        return m_impl->hook.get();
    }

    listener::operator spa_hook *() const &
    {
        return get();
    }
} // namespace pipewire
