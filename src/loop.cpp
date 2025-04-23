#include "loop.hpp"
#include "utils/check.hpp"

#include <mutex>
#include <memory>

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct main_loop::impl
    {
        pw_unique_ptr<raw_type> main_loop;
    };

    main_loop::main_loop(deleter<raw_type> deleter, raw_type *raw)
        : m_impl(std::make_unique<impl>(pw_unique_ptr<raw_type>{raw, deleter}))
    {
    }

    main_loop::~main_loop() = default;

    void main_loop::run() const
    {
        pw_main_loop_run(m_impl->main_loop.get());
    }

    void main_loop::quit() const
    {
        pw_main_loop_quit(m_impl->main_loop.get());
    }

    main_loop::raw_type *main_loop::get() const
    {
        return m_impl->main_loop.get();
    }

    pw_loop *main_loop::loop() const
    {
        return pw_main_loop_get_loop(m_impl->main_loop.get());
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

        return from(loop);
    }

    std::shared_ptr<main_loop> main_loop::from(raw_type *raw)
    {
        return std::shared_ptr<main_loop>(new main_loop{pw_main_loop_destroy, raw});
    }

    std::shared_ptr<main_loop> main_loop::view(raw_type *raw)
    {
        return std::shared_ptr<main_loop>(new main_loop{view_deleter<raw_type>, raw});
    }
} // namespace pipewire
