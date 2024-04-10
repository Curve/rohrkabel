#include "spa/pod/pod.hpp"
#include "spa/pod/object/body.hpp"

#include <cassert>
#include <spa/debug/pod.h>
#include <spa/pod/builder.h>

namespace pipewire::spa
{
    struct pod::impl
    {
        const spa_type_info *type;
        std::shared_ptr<spa_pod> pod;
    };

    pod::~pod() = default;

    pod::pod(std::shared_ptr<spa_pod> pod) : m_impl(std::make_unique<impl>())
    {
        m_impl->type = spa_debug_type_find(nullptr, pod->type);
        m_impl->pod  = std::move(pod);
    }

    pod::pod(pod &&other) noexcept : m_impl(std::move(other.m_impl)) {}

    pod::pod(const pod &other) noexcept : m_impl(std::make_unique<impl>())
    {
        *m_impl = *other.m_impl;
    }

    pod &pod::operator=(pod &&other) noexcept
    {
        m_impl = std::move(other.m_impl);
        return *this;
    }

    pod &pod::operator=(const pod &other) noexcept
    {
        if (&other != this)
        {
            *m_impl = *other.m_impl;
        }

        return *this;
    }

    pod_type pod::type() const
    {
        return static_cast<pod_type>(m_impl->pod->type);
    }

    std::size_t pod::size() const
    {
        return m_impl->pod->size;
    }

    std::string pod::name() const
    {
        return m_impl->type->name;
    }

    template <>
    pod_object_body pod::body<pod_object_body>() const
    {
        auto *body = reinterpret_cast<spa_pod_object_body *>(SPA_POD_BODY(m_impl->pod.get()));
        return pod_object_body::view(body, size());
    }

    template <>
    bool pod::read() const
    {
        assert(type() == pod_type::boolean);
        return reinterpret_cast<spa_pod_bool *>(m_impl->pod.get())->value;
    }

    template <>
    float pod::read() const
    {
        assert(type() == pod_type::num_float);
        return reinterpret_cast<spa_pod_float *>(m_impl->pod.get())->value;
    }

    template <>
    std::string pod::read() const
    {
        assert(type() == pod_type::string);
        return {reinterpret_cast<const char *>(SPA_POD_CONTENTS(spa_pod_string, m_impl->pod.get()))};
    }

    template <>
    void pod::write(bool value)
    {
        assert(type() == pod_type::boolean);
        reinterpret_cast<spa_pod_bool *>(m_impl->pod.get())->value = value;
    }

    template <>
    void pod::write(float value)
    {
        assert(type() == pod_type::num_float);
        reinterpret_cast<spa_pod_float *>(m_impl->pod.get())->value = value;
    }

    spa_pod *pod::get() const
    {
        return m_impl->pod.get();
    }

    const spa_type_info *pod::type_info() const
    {
        return m_impl->type;
    }

    pod::operator spa_pod *() const &
    {
        return get();
    }

    pod pod::view(spa_pod *pod)
    {
        return {std::shared_ptr<spa_pod>(pod, [](auto *) {})};
    }

    pod pod::copy(const spa_pod *pod)
    {
        auto deleter = [](spa_pod *pod)
        {
            free(pod); // NOLINT(*-malloc)
        };

        return {std::shared_ptr<spa_pod>(spa_pod_copy(pod), deleter)};
    }
} // namespace pipewire::spa
