#include "spa/pod/object/body.hpp"

#include <stdexcept>
#include <spa/pod/pod.h>
#include <spa/pod/iter.h>
#include <spa/debug/pod.h>

namespace pipewire::spa
{
    struct pod_object_body::impl
    {
        std::size_t size;
        spa_pod_object_body *body;
        const spa_type_info *type;
    };

    pod_object_body::~pod_object_body() = default;

    pod_object_body::pod_object_body(const pod &pod) : m_impl(std::make_unique<impl>())
    {
        m_impl->size = pod.size();
        m_impl->body = reinterpret_cast<spa_pod_object_body *>(SPA_POD_BODY(pod.get())); // NOLINT
        m_impl->type = spa_debug_type_find(nullptr, m_impl->body->type);
    }

    pod_object_body::pod_object_body(pod_object_body &&body) noexcept : m_impl(std::move(body.m_impl)) {}

    pod_object_body &pod_object_body::operator=(pod_object_body &&body) noexcept
    {
        m_impl = std::move(body.m_impl);
        return *this;
    }

    bool pod_object_body::has(std::uint32_t key) const
    {
        spa_pod_prop *iter{};
        SPA_POD_OBJECT_BODY_FOREACH(m_impl->body, m_impl->size, iter)
        {
            if (iter->key == key)
            {
                return true;
            }
        }

        return false;
    }

    pod_prop pod_object_body::at(std::uint32_t key) const
    {
        spa_pod_prop *iter{};
        SPA_POD_OBJECT_BODY_FOREACH(m_impl->body, m_impl->size, iter)
        {
            if (iter->key == key)
            {
                return {iter, m_impl->type};
            }
        }

        throw std::out_of_range("object with key does not exist");
    }

    pod_object_body_iterator pod_object_body::end() const
    {
        spa_pod_prop *end{};
        SPA_POD_OBJECT_BODY_FOREACH(m_impl->body, m_impl->size, end) {}

        return {m_impl->size, end, *this};
    }

    pod_object_body_iterator pod_object_body::begin() const
    {
        return {m_impl->size, spa_pod_prop_first(m_impl->body), *this};
    }

    pod_type pod_object_body::type() const
    {
        return static_cast<pod_type>(m_impl->body->type);
    }

    std::uint32_t pod_object_body::id() const
    {
        return m_impl->body->id;
    }

    spa_pod_object_body *pod_object_body::get() const
    {
        return m_impl->body;
    }

    const spa_type_info *pod_object_body::get_type() const
    {
        return m_impl->type;
    }
} // namespace pipewire::spa