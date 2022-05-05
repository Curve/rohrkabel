#include "listener.hpp"
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct listener::impl
    {
        spa_hook hook;
    };

    listener::~listener()
    {
        if (m_impl)
        {
            spa_hook_remove(&m_impl->hook);
        }
    }

    listener::listener() : m_impl(std::make_unique<impl>())
    {
        spa_zero(m_impl->hook);
    }

    listener::listener(listener &&listener) noexcept : m_impl(std::move(listener.m_impl)) {}

    listener &listener::operator=(listener &&listener) noexcept
    {
        m_impl = std::move(listener.m_impl);
        return *this;
    }

    spa_hook &listener::get()
    {
        return m_impl->hook;
    }
} // namespace pipewire