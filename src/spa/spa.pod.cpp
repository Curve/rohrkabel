#include "spa/pod/pod.hpp"

#include "spa/pod/prop.hpp"
#include "spa/pod/object/object.hpp"

#include <cassert>
#include <spa/debug/pod.h>
#include <spa/pod/builder.h>

namespace pipewire::spa
{
    // TODO: Make move-only?

    struct pod::impl
    {
        std::shared_ptr<raw_type> pod;
    };

    pod::~pod() = default;

    pod::pod(std::shared_ptr<raw_type> pod) : m_impl(std::make_unique<impl>())
    {
        m_impl->pod = std::move(pod);
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

    std::optional<pod_prop> pod::find(prop key) const
    {
        const auto *prop = spa_pod_find_prop(m_impl->pod.get(), nullptr, static_cast<std::uint32_t>(key));

        if (!prop)
        {
            return std::nullopt;
        }

        // TODO: const_cast :>(
        return pod_prop::view(const_cast<spa_pod_prop *>(prop)); // NOLINT(*-const-cast)
    }

    std::optional<pod_prop> pod::find_recursive(prop key) const
    {
        auto find_recursive = [key](const auto &pod, auto &self) -> std::optional<pod_prop> // NOLINT(*-recursion)
        {
            if (auto ret = pod.find(key); ret)
            {
                return ret;
            }

            if (pod.type() != spa::type::object)
            {
                return std::nullopt;
            }

            auto object = pod.template as<spa::pod_object>();

            for (const auto &child : object)
            {
                auto ret = self(child.value(), self);

                if (!ret)
                {
                    continue;
                }

                return ret;
            }

            return std::nullopt;
        };

        return find_recursive(*this, find_recursive);
    }

    spa::type pod::type() const
    {
        return static_cast<spa::type>(m_impl->pod->type);
    }

    std::size_t pod::size() const
    {
        return m_impl->pod->size;
    }

    template <>
    bool pod::as() const
    {
        assert(type() == spa::type::boolean);
        return reinterpret_cast<spa_pod_bool *>(m_impl->pod.get())->value;
    }

    template <>
    float pod::as() const
    {
        assert(type() == spa::type::num_float);
        return reinterpret_cast<spa_pod_float *>(m_impl->pod.get())->value;
    }

    template <>
    pod_object pod::as<pod_object>() const
    {
        assert(type() == spa::type::object);
        return pod_object::view(reinterpret_cast<spa_pod_object *>(m_impl->pod.get()));
    }
    template <>
    std::string pod::as() const
    {
        assert(type() == spa::type::string);
        return {reinterpret_cast<const char *>(SPA_POD_CONTENTS(spa_pod_string, m_impl->pod.get()))};
    }

    template <>
    std::vector<void *> pod::as() const
    {
        assert(type() == spa::type::array);

        std::vector<void *> rtn;

        auto *array = reinterpret_cast<spa_pod_array *>(m_impl->pod.get());
        void *iter  = {};

        SPA_POD_ARRAY_FOREACH(array, iter)
        {
            rtn.emplace_back(iter);
        }

        return rtn;
    }

    template <>
    void pod::write(bool value)
    {
        assert(type() == spa::type::boolean);
        reinterpret_cast<spa_pod_bool *>(m_impl->pod.get())->value = value;
    }

    template <>
    void pod::write(float value)
    {
        assert(type() == spa::type::num_float);
        reinterpret_cast<spa_pod_float *>(m_impl->pod.get())->value = value;
    }

    pod::raw_type *pod::get() const
    {
        return m_impl->pod.get();
    }

    pod::operator raw_type *() const &
    {
        return get();
    }

    pod pod::view(raw_type *pod)
    {
        auto deleter = [](raw_type *) {
        };

        return {std::shared_ptr<raw_type>(pod, deleter)};
    }

    pod pod::copy(const raw_type *pod)
    {
        auto deleter = [](raw_type *pod) {
            free(pod); // NOLINT(*-malloc)
        };

        return {std::shared_ptr<raw_type>(spa_pod_copy(pod), deleter)};
    }
} // namespace pipewire::spa
