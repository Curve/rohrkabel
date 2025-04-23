#include "context.hpp"

#include "loop.hpp"
#include "utils/check.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct context::impl
    {
        pw_unique_ptr<raw_type> context;
        std::shared_ptr<main_loop> loop;
    };

    context::context(deleter<raw_type> deleter, raw_type *raw, std::shared_ptr<main_loop> loop)
        : m_impl(std::make_unique<impl>(pw_unique_ptr<raw_type>{raw, deleter}, std::move(loop)))
    {
    }

    context::~context() = default;

    context::raw_type *context::get() const
    {
        return m_impl->context.get();
    }

    std::shared_ptr<main_loop> context::loop() const
    {
        return m_impl->loop;
    }

    context::operator raw_type *() const &
    {
        return get();
    }

    std::shared_ptr<context> context::create(std::shared_ptr<main_loop> loop)
    {
        auto *ctx = pw_context_new(loop->loop(), nullptr, 0);
        check(ctx, "Failed to create context");

        if (!ctx)
        {
            return nullptr;
        }

        return from(ctx, std::move(loop));
    }

    std::shared_ptr<context> context::from(raw_type *raw, std::shared_ptr<main_loop> loop)
    {
        return std::shared_ptr<context>(new context{pw_context_destroy, raw, std::move(loop)});
    }

    std::shared_ptr<context> context::view(raw_type *raw, std::shared_ptr<main_loop> loop)
    {
        return std::shared_ptr<context>(new context{view_deleter<raw_type>, raw, std::move(loop)});
    }
} // namespace pipewire
