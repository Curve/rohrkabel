#include "spa/pod/object/iterator.hpp"

#include <spa/pod/iter.h>

namespace pipewire::spa
{
    using iterator = pod_object::iterator;

    struct iterator::impl
    {
        spa_pod_prop *iter;
        const pod_object *object;

      public:
        bool operator==(const impl &) const = default;
    };

    iterator::~iterator() = default;

    iterator::iterator() : m_impl(std::make_unique<impl>()) {}

    iterator::iterator(const iterator &other) : iterator()
    {
        *m_impl = *other.m_impl;
    }

    iterator::iterator(const pod_object *object) : iterator()
    {
        m_impl->iter   = spa_pod_prop_first(&object->get()->body);
        m_impl->object = object;
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
        return pod_prop::view(m_impl->iter);
    }

    bool iterator::operator==(const iterator &other) const
    {
        return *m_impl == *other.m_impl;
    }

    bool iterator::operator==(const sentinel &) const
    {
        auto *body = &m_impl->object->get()->body;
        auto &pod  = m_impl->object->get()->pod;

        return !spa_pod_prop_is_inside(body, pod.size, m_impl->iter);
    }
} // namespace pipewire::spa
