#include "loop.hpp"
#include "context.hpp"
#include "core/core.hpp"
#include "utils/assert.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct context::impl
    {
        pw_context *context;
        std::shared_ptr<main_loop> loop;
        std::shared_ptr<pipewire::core> core;
    };

    context::~context()
    {
        pw_context_destroy(m_impl->context);
    }

    context::context() : m_impl(std::make_unique<impl>()) {}

    std::shared_ptr<core> context::core()
    {
        if (!m_impl->core)
        {
            m_impl->core = std::shared_ptr<pipewire::core>(new pipewire::core{shared_from_this()});
        }

        return m_impl->core;
    }

    pw_context *context::get() const
    {
        return m_impl->context;
    }

    std::shared_ptr<main_loop> context::loop() const
    {
        return m_impl->loop;
    }

    context::operator pw_context *() const &
    {
        return get();
    }

    std::shared_ptr<context> context::create(std::shared_ptr<main_loop> loop)
    {
        auto rtn = std::unique_ptr<context>(new context);

        rtn->m_impl->context = pw_context_new(loop->loop(), nullptr, 0);
        rtn->m_impl->loop    = std::move(loop);

        check(rtn->m_impl->context, "Failed to create context");

        return rtn;
    }
} // namespace pipewire
