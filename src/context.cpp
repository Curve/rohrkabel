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

    context::context(loop &loop) : m_loop(loop), m_impl(std::make_unique<impl>())
    {
        m_impl->context = pw_context_new(loop.get(), nullptr, 0);
        assert((void("Failed to create context"), m_impl->context));
    }

    loop &context::get_loop()
    {
        return m_loop;
    }

    pw_context *context::get() const
    {
        return m_impl->context;
    }

} // namespace pipewire