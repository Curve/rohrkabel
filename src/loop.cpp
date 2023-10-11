#include "loop.hpp"
#include "utils/assert.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct main_loop::impl
    {
        pw_main_loop *main_loop;
    };

    main_loop::~main_loop()
    {
        pw_main_loop_destroy(m_impl->main_loop);
    }

    main_loop::main_loop() : m_impl(std::make_unique<impl>())
    {
        m_impl->main_loop = pw_main_loop_new(nullptr);
        check(m_impl->main_loop, "Failed to create main_loop");
    }

    void main_loop::quit() const
    {
        pw_main_loop_quit(m_impl->main_loop);
    }

    void main_loop::run() const
    {
        pw_main_loop_run(m_impl->main_loop);
    }

    pw_loop *main_loop::loop() const
    {
        return pw_main_loop_get_loop(m_impl->main_loop);
    }

    pw_main_loop *main_loop::get() const
    {
        return m_impl->main_loop;
    }

    main_loop::operator pw_main_loop *() const &
    {
        return get();
    }

    std::shared_ptr<main_loop> main_loop::create()
    {
        static auto once = false;

        if (!once)
        {
            pw_init(nullptr, nullptr);
            once = true;
        }

        return std::unique_ptr<main_loop>(new main_loop);
    }
} // namespace pipewire
