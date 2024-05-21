#include "loop.hpp"
#include "utils/check.hpp"

#include <mutex>
#include <memory>

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct main_loop::impl
    {
        raw_type *main_loop;
    };

    main_loop::~main_loop()
    {
        pw_main_loop_destroy(m_impl->main_loop);
    }

    main_loop::main_loop() : m_impl(std::make_unique<impl>()) {}

    void main_loop::quit() const
    {
        pw_main_loop_quit(m_impl->main_loop);
    }

    void main_loop::run() const
    {
        pw_main_loop_run(m_impl->main_loop);
    }

    main_loop::raw_type *main_loop::get() const
    {
        return m_impl->main_loop;
    }

    pw_loop *main_loop::loop() const
    {
        return pw_main_loop_get_loop(m_impl->main_loop);
    }

    main_loop::operator raw_type *() const &
    {
        return get();
    }

    std::shared_ptr<main_loop> main_loop::create()
    {
        static std::once_flag flag;

        std::call_once(flag, [] {
            pw_init(nullptr, nullptr);
        });

        auto *loop = pw_main_loop_new(nullptr);
        check(loop, "Failed to create main_loop");

        if (!loop)
        {
            return nullptr;
        }

        auto rtn = std::unique_ptr<main_loop>(new main_loop);

        rtn->m_impl->main_loop = loop;

        return rtn;
    }
} // namespace pipewire
