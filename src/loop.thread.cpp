#include "loop/thread.hpp"

#include <pipewire/pipewire.h>
#include <pipewire/thread-loop.h>

namespace pipewire
{
    struct thread_loop::impl
    {
        pw_thread_loop *loop;
    };

    thread_loop::~thread_loop()
    {
        pw_thread_loop_destroy(m_impl->loop);
    }

    thread_loop::thread_loop(const std::string &name) : m_impl(std::make_unique<impl>())
    {
        m_impl->loop = pw_thread_loop_new(name.c_str(), nullptr);
    }

    void thread_loop::run() const
    {
        pw_thread_loop_start(m_impl->loop);
    }

    void thread_loop::quit() const
    {
        pw_thread_loop_stop(m_impl->loop);
    }

    void thread_loop::lock() const
    {
        pw_thread_loop_lock(m_impl->loop);
    }

    void thread_loop::unlock() const
    {
        pw_thread_loop_unlock(m_impl->loop);
    }

    pw_loop *thread_loop::get() const
    {
        return pw_thread_loop_get_loop(m_impl->loop);
    }

    pw_thread_loop *thread_loop::get_thread_loop() const
    {
        return m_impl->loop;
    }
} // namespace pipewire