#include "spa/pod/object/object.hpp"
#include "spa/pod/object/iterator.hpp"

#include <spa/pod/pod.h>
#include <spa/pod/iter.h>
#include <spa/debug/pod.h>

namespace pipewire::spa
{
    struct pod_object::impl
    {
        spa_pod_object *object;
    };

    pod_object::~pod_object() = default;

    pod_object::pod_object(raw_type *object) : m_impl(std::make_unique<impl>())
    {
        m_impl->object = object;
    }

    pod_object::pod_object(pod_object &&other) noexcept : m_impl(std::move(other.m_impl)) {}

    pod_object &pod_object::operator=(pod_object &&other) noexcept
    {
        m_impl = std::move(other.m_impl);
        return *this;
    }

    spa::pod pod_object::pod() const
    {
        return spa::pod::view(&m_impl->object->pod);
    }

    spa::type pod_object::type() const
    {
        return static_cast<spa::type>(m_impl->object->body.type);
    }

    std::uint32_t pod_object::id() const
    {
        return m_impl->object->body.id;
    }

    pod_object::sentinel pod_object::end() const // NOLINT(*-static)
    {
        return {};
    }

    pod_object::iterator pod_object::begin() const
    {
        return {this};
    }

    pod_object::raw_type *pod_object::get() const
    {
        return m_impl->object;
    }

    pod_object::operator raw_type *() const &
    {
        return get();
    }

    pod_object pod_object::view(raw_type *object)
    {
        return {object};
    }
} // namespace pipewire::spa
