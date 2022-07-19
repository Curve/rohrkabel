#include "spa/pod/pod.hpp"
#include "spa/pod/object/body.hpp"

#include <cassert>
#include <spa/debug/pod.h>
#include <spa/pod/builder.h>

namespace pipewire::spa
{
    struct pod::impl
    {
        spa_pod *pod;
        bool is_copy{false};
        const spa_type_info *type;
    };

    pod::~pod()
    {
        if (m_impl && m_impl->is_copy)
        {
            free(m_impl->pod);
        }
    }

    pod::pod(spa_pod *pod) : m_impl(std::make_unique<impl>())
    {
        m_impl->pod = pod;
        m_impl->type = spa_debug_type_find(nullptr, pod->type);
    }

    pod::pod(const spa_pod *pod) : spa::pod(spa_pod_copy(pod))
    {
        m_impl->is_copy = true;
    }

    pod::pod(pod &&pod) noexcept : m_impl(std::move(pod.m_impl)) {}

    pod &pod::operator=(pod &&pod) noexcept
    {
        m_impl = std::move(pod.m_impl);
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

    template <> bool pod::as() const
    {
        assert(type() == pod_type::boolean);
        return reinterpret_cast<spa_pod_bool *>(m_impl->pod)->value;
    }

    template <> float pod::as() const
    {
        assert(type() == pod_type::num_float);
        return reinterpret_cast<spa_pod_float *>(m_impl->pod)->value;
    }

    template <> std::string pod::as() const
    {
        assert(type() == pod_type::string);

        // NOLINTNEXTLINE
        const auto *content = reinterpret_cast<const char *>(SPA_POD_CONTENTS(spa_pod_string, m_impl->pod));

        return {content};
    }

    template <> pod_object_body pod::body<pod_object_body>() const
    {
        return {*this};
    }

    template <> void pod::write(const bool &value)
    {
        assert(type() == pod_type::boolean);
        reinterpret_cast<spa_pod_bool *>(m_impl->pod)->value = value;
    }

    template <> void pod::write(const float &value)
    {
        assert(type() == pod_type::num_float);
        reinterpret_cast<spa_pod_float *>(m_impl->pod)->value = value;
    }

    spa_pod *pod::get() const
    {
        return m_impl->pod;
    }

    const spa_type_info *pod::get_type() const
    {
        return m_impl->type;
    }
} // namespace pipewire::spa