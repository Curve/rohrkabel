#include "spa/pod/object/iterator.hpp"

#include <spa/pod/iter.h>

namespace pipewire::spa
{
    using iterator = pod_object_body::iterator;

    struct iterator::impl
    {
        spa_pod_prop *iter;
        const pod_object_body *body;
    };

    iterator::~iterator() = default;

    iterator::iterator() : m_impl(std::make_unique<impl>()) {}

    iterator::iterator(const iterator &other) : iterator()
    {
        *m_impl = *other.m_impl;
    }

    iterator::iterator(const pod_object_body *body) : iterator()
    {
        m_impl->iter = spa_pod_prop_first(body->get());
        m_impl->body = body;
    }

    iterator &iterator::operator=(const iterator &other)
    {
        if (&other != this)
        {
            *m_impl = *other.m_impl;
        }

        return *this;
    }

    iterator &iterator::operator++()
    {
        static constexpr auto end = sentinel{};

        if (*this != end)
        {
            m_impl->iter = spa_pod_prop_next(m_impl->iter);
        }

        return *this;
    }

    iterator iterator::operator++(int) const &
    {
        iterator copy{*this};
        ++copy;

        return copy;
    }

    pod_prop iterator::operator*() const
    {
        return pod_prop::view(m_impl->iter, m_impl->body->type_info());
    }

    bool iterator::operator==(const iterator &other) const
    {
        return m_impl->iter == other.m_impl->iter && m_impl->body == other.m_impl->body;
    }

    bool iterator::operator==(const sentinel &) const
    {
        return !spa_pod_prop_is_inside(m_impl->body->get(), m_impl->body->size(), m_impl->iter);
    }
} // namespace pipewire::spa
