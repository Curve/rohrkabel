#include "spa/pod/object/body.hpp"
#include "spa/pod/object/iterator.hpp"

#include <spa/pod/iter.h>

namespace pipewire::spa
{
    struct pod_object_body_iterator::impl
    {
        std::size_t size;
        spa_pod_prop *iter;

        spa_pod_object_body *body;
        const spa_type_info *type;
    };

    pod_object_body_iterator::~pod_object_body_iterator() = default;

    pod_object_body_iterator::pod_object_body_iterator(std::size_t size, spa_pod_prop *iter, const pod_object_body &body) : m_impl(std::make_unique<impl>())
    {
        m_impl->size = size;
        m_impl->iter = iter;

        m_impl->body = body.get();
        m_impl->type = body.get_type();
    }

    pod_prop pod_object_body_iterator::operator*()
    {
        return {m_impl->iter, m_impl->type};
    }

    std::unique_ptr<pod_prop> pod_object_body_iterator::operator->()
    {
        return std::make_unique<pod_prop>(m_impl->iter, m_impl->type);
    }

    pod_object_body_iterator &pod_object_body_iterator::operator++()
    {
        if (spa_pod_prop_is_inside(m_impl->body, m_impl->size, m_impl->iter))
        {
            m_impl->iter = spa_pod_prop_next(m_impl->iter);
        }

        return *this;
    }

    bool pod_object_body_iterator::operator!=(const pod_object_body_iterator &other)
    {
        return m_impl->iter != other.m_impl->iter;
    }
} // namespace pipewire::spa