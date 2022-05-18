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

    pod_prop::pod_prop(pod_prop &&prop) noexcept : m_impl(std::move(prop.m_impl)) {}

    pod_prop::pod_prop(spa_pod_prop *prop, const spa_type_info *type) : m_impl(std::make_unique<impl>())
    {
        m_impl->prop = prop;
        m_impl->type = spa_debug_type_find(type->values, prop->key);
    }

    pod_prop &pod_prop::operator=(pod_prop &&prop) noexcept
    {
        m_impl = std::move(prop.m_impl);
        return *this;
    }

    pod pod_prop::value() const
    {
        return {&m_impl->prop->value};
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

    const spa_type_info *pod_prop::get_type() const
    {
        return m_impl->type;
    }
} // namespace pipewire::spa