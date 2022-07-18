#include "loop/main.hpp"

#include <thread>
#include <cassert>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct main_loop::impl
    {
        spa_source *source;
        pw_main_loop *main_loop;
        static inline std::once_flag flag;
    };

    void main_loop::emit_event() const
    {
        // NOLINTNEXTLINE
        pw_loop_signal_event(get(), m_impl->source);
    }

    main_loop::main_loop() : m_impl(std::make_unique<impl>())
    {
        std::call_once(impl::flag, [] { pw_init(nullptr, nullptr); });

        m_impl->main_loop = pw_main_loop_new(nullptr);
        assert((void("Failed to create main_loop"), m_impl->main_loop));

        // NOLINTNEXTLINE
        m_impl->source = pw_loop_add_event(
            get(),
            [](void *_thiz, [[maybe_unused]] std::uint64_t count) {
                auto *thiz = reinterpret_cast<main_loop *>(_thiz);
                std::lock_guard guard(thiz->m_queue_mutex);

                while (!thiz->m_queue.empty())
                {
                    auto &&function = std::move(thiz->m_queue.front());
                    thiz->m_queue.pop();
                    function();
                }
            },
            this);
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

    pw_loop *main_loop::get() const
    {
        return pw_main_loop_get_loop(m_impl->main_loop);
    }

    pw_main_loop *main_loop::get_main_loop() const
    {
        return m_impl->main_loop;
    }
} // namespace pipewire