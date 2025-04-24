#include "spa/pod/pod.hpp"
#include "spa/pod/prop.hpp"

#include <spa/pod/pod.h>

namespace pipewire::spa
{
    struct pod_prop::impl
    {
        pw_unique_ptr<raw_type> prop;
    };

    pod_prop::pod_prop(deleter<raw_type> deleter, raw_type *prop)
        : m_impl(std::make_unique<impl>(pw_unique_ptr<raw_type>{prop, deleter}))
    {
    }

    pod_prop::pod_prop(pod_prop &&) noexcept = default;

    pod_prop &pod_prop::operator=(pod_prop &&) noexcept = default;

    pod_prop::~pod_prop() = default;

    pod pod_prop::value() const &
    {
        return pod::view(&m_impl->prop->value);
    }

    std::uint32_t pod_prop::flags() const
    {
        return m_impl->prop->flags;
    }

    enum_value<prop> pod_prop::key() const
    {
        return m_impl->prop->key;
    }

    pod_prop::raw_type *pod_prop::get() const
    {
        return m_impl->prop.get();
    }

    pod_prop::operator raw_type *() const &
    {
        return get();
    }

    pod_prop pod_prop::view(raw_type *prop)
    {
        return {view_deleter<raw_type>, prop};
    }
} // namespace pipewire::spa
