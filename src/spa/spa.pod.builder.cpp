#include "spa/pod/builder.hpp"

#include <spa/pod/builder.h>
#include <spa/pod/dynamic.h>

#include <list>

namespace pipewire::spa
{
    struct pod_builder::impl
    {
        pw_unique_ptr<spa_pod_builder> builder;
        std::list<spa_pod_frame> frames;
    };

    pod_builder::pod_builder(deleter<raw_type> deleter, raw_type *raw)
        : m_impl(std::make_unique<impl>(pw_unique_ptr<raw_type>{raw, deleter}))
    {
    }

    pod_builder::pod_builder(pod_builder &&) noexcept = default;

    pod_builder &pod_builder::operator=(pod_builder &&) noexcept = default;

    pod_builder::~pod_builder() = default;

    template <>
    void pod_builder::write(bool value)
    {
        spa_pod_builder_bool(m_impl->builder.get(), value);
    }

    template <>
    void pod_builder::write(int value)
    {
        spa_pod_builder_int(m_impl->builder.get(), value);
    }

    template <>
    void pod_builder::write(long value)
    {
        spa_pod_builder_long(m_impl->builder.get(), value);
    }

    template <>
    void pod_builder::write(float value)
    {
        spa_pod_builder_float(m_impl->builder.get(), value);
    }

    template <>
    void pod_builder::write(double value)
    {
        spa_pod_builder_double(m_impl->builder.get(), value);
    }

    void pod_builder::push_object(std::uint32_t type, std::uint32_t id)
    {
        auto *const frame = std::addressof(m_impl->frames.emplace_back());
        spa_pod_builder_push_object(m_impl->builder.get(), frame, type, id);
    }

    std::optional<pod> pod_builder::pop()
    {
        auto *const frame  = std::addressof(m_impl->frames.back());
        auto *const result = spa_pod_builder_pop(m_impl->builder.get(), frame);

        m_impl->frames.pop_back();

        if (!result)
        {
            return std::nullopt;
        }

        return pod::copy(static_cast<spa_pod *>(result));
    }

    void pod_builder::prop(std::uint32_t prop, std::uint32_t flags)
    {
        spa_pod_builder_prop(m_impl->builder.get(), prop, flags);
    }

    pod_builder::raw_type *pod_builder::get() const
    {
        return m_impl->builder.get();
    }

    pod_builder::raw_type *pod_builder::release() &&
    {
        return m_impl->builder.release();
    }

    pod_builder::operator raw_type *() const &
    {
        return get();
    }

    pod_builder pod_builder::create()
    {
        static const auto deleter = [](auto *value)
        {
            auto *const builder = reinterpret_cast<spa_pod_dynamic_builder *>(value);
            spa_pod_dynamic_builder_clean(builder);
            delete builder;
        };

        auto *const builder = new spa_pod_dynamic_builder{};
        spa_pod_dynamic_builder_init(builder, nullptr, 0, 4096);

        return {deleter, reinterpret_cast<spa_pod_builder *>(builder)};
    }

    pod_builder pod_builder::view(raw_type *raw)
    {
        return {view_deleter<raw_type>, raw};
    }
} // namespace pipewire::spa
