#include "main_loop.hpp"

#include <mutex>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct main_loop::impl
    {
        pw_main_loop *main_loop;
        static inline std::once_flag init_flag;
    };

    main_loop::main_loop() : m_impl(std::make_unique<impl>())
    {
        std::call_once(impl::init_flag, [] { pw_init(nullptr, nullptr); });
        m_impl->main_loop = pw_main_loop_new(nullptr);
    }

    main_loop::~main_loop()
    {
        pw_main_loop_destroy(m_impl->main_loop);
    }

    void main_loop::run() const
    {
        pw_main_loop_run(m_impl->main_loop);
    }

    void main_loop::quit() const
    {
        pw_main_loop_quit(m_impl->main_loop);
    }

    pw_main_loop *main_loop::get() const
    {
        return m_impl->main_loop;
    }
} // namespace pipewire