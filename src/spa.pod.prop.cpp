#include "spa/pod/pod.hpp"
#include "spa/pod/prop.hpp"

#include <spa/pod/pod.h>
#include <spa/debug/pod.h>

namespace pipewire::spa
{
    struct pod_prop::impl
    {
        spa_pod_prop *prop;
        const spa_type_info *type;
    };

    pod_prop::~pod_prop() = default;

    pod_prop::pod_prop() : m_impl(std::make_unique<impl>()) {}

    pod_prop::pod_prop(pod_prop &&other) noexcept : m_impl(std::move(other.m_impl)) {}

    pod_prop::pod_prop(const pod_prop &other) noexcept : pod_prop()
    {
        *m_impl = *other.m_impl;
    }

    pod_prop &pod_prop::operator=(pod_prop &&other) noexcept
    {
        m_impl = std::move(other.m_impl);
        return *this;
    }

    pod_prop &pod_prop::operator=(const pod_prop &other) noexcept
    {
        if (&other != this)
        {
            *m_impl = *other.m_impl;
        }

        return *this;
    }

    pod pod_prop::value() const
    {
        return pod::view(&m_impl->prop->value);
    }

    std::string pod_prop::name() const
    {
        return m_impl->type->name;
    }

    std::uint32_t pod_prop::key() const
    {
        return m_impl->prop->key;
    }

    std::uint32_t pod_prop::flags() const
    {
        return m_impl->prop->flags;
    }

    spa_pod_prop *pod_prop::get() const
    {
        return m_impl->prop;
    }

    const spa_type_info *pod_prop::type_info() const
    {
        return m_impl->type;
    }

    pod_prop pod_prop::view(spa_pod_prop *prop, const spa_type_info *type)
    {
        pod_prop rtn;

        rtn.m_impl->type = spa_debug_type_find(type->values, prop->key);
        rtn.m_impl->prop = prop;

        return rtn;
    }
} // namespace pipewire::spa