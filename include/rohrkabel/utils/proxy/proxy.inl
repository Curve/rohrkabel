#pragma once
#include "proxy.hpp"

#include <cassert>

namespace pipewire
{
    template <typename T, typename U> safe_proxy<T, U>::~safe_proxy()
    {
        m_loop.emit_cleanup();
    }

    template <typename T, typename U> safe_proxy<T, U>::safe_proxy(std::shared_ptr<T> instance, main_loop &loop) : m_loop(loop), m_instance(instance) {}

    template <typename T, typename U> const T *safe_proxy<T, U>::operator->() const
    {
        return m_instance.get();
    }

    template <typename T, typename U> T &safe_proxy<T, U>::get_safe()
    {
        assert((void("get_safe should only be used in a safe context!"), m_loop.is_safe()));
        return *m_instance;
    }
} // namespace pipewire