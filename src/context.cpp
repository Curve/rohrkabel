#include "context.hpp"

#include <cassert>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct context::impl
    {
        pw_context *context;
    };

    context::~context()
    {
        pw_context_destroy(m_impl->context);
    }

    context::context(main_loop &main_loop) : m_main_loop(main_loop), m_impl(std::make_unique<impl>())
    {
        m_impl->context = pw_context_new(pw_main_loop_get_loop(main_loop.get()), nullptr, 0);
        assert((void("Failed to create context"), m_impl->context));
    }

    pw_context *context::get() const
    {
        return m_impl->context;
    }

    main_loop &context::get_main_loop()
    {
        return m_main_loop;
    }
} // namespace pipewire