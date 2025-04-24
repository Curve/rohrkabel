#include "spa/pod/object.hpp"

#include <spa/pod/pod.h>
#include <spa/pod/iter.h>

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

    pod_object::pod_object(pod_object &&) noexcept = default;

    pod_object &pod_object::operator=(pod_object &&) noexcept = default;

    spa::pod pod_object::pod() const &
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

    coco::generator<pod_prop> pod_object::props() const
    {
        auto *body      = &m_impl->object->body;
        const auto size = m_impl->object->pod.size;

        for (auto *it = spa_pod_prop_first(&m_impl->object->body); spa_pod_prop_is_inside(body, size, it);
             it       = spa_pod_prop_next(it))
        {
            co_yield pod_prop::view(it);
        }
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
