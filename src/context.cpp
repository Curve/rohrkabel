#include "context.hpp"

#include <pipewire/pipewire.h>
#include <pipewire/impl-module.h>

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

    res<context::module, std::error_code> context::load(module_options opts)
    {
        auto *const props = std::move(opts.props).release();
        auto *const mod   = pw_context_load_module(m_impl->context.get(), opts.name.c_str(), opts.args.c_str(), props);

        if (!mod)
        {
            return std::unexpected{std::make_error_code(static_cast<std::errc>(errno))};
        }

        return module::from(mod);
    }

    std::shared_ptr<main_loop> context::loop() const
    {
        return m_impl->loop;
    }

    context::raw_type *context::get() const
    {
        return m_impl->context.get();
    }

    context::raw_type *context::release() &&
    {
        return m_impl->context.release();
    }

    context::operator raw_type *() const &
    {
        return get();
    }

    res<std::shared_ptr<context>, std::error_code> context::create(std::shared_ptr<main_loop> loop)
    {
        auto *const ctx = pw_context_new(loop->loop(), nullptr, 0);

        if (!ctx)
        {
            return std::unexpected{std::make_error_code(static_cast<std::errc>(errno))};
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
