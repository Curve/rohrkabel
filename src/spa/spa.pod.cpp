#include "spa/pod/pod.hpp"

#include "spa/pod/prop.hpp"
#include "spa/pod/object/object.hpp"

#include <cassert>
#include <spa/debug/pod.h>
#include <spa/pod/builder.h>

namespace pipewire::spa
{
    struct pod::impl
    {
        pw_unique_ptr<raw_type> pod;
    };

    pod::~pod() = default;

    pod::pod(deleter<raw_type> deleter, raw_type *pod) : m_impl(std::make_unique<impl>(pw_unique_ptr<raw_type>{pod, deleter})) {}

    pod::pod(const pod &other) : m_impl(std::move(copy(other.get()).m_impl)) {}

    pod::pod(pod &&other) noexcept : m_impl(std::move(other.m_impl)) {}

    pod &pod::operator=(const pod &other)
    {
        if (&other != this)
        {
            m_impl = std::move(copy(other.get()).m_impl);
        }

        return *this;
    }

    pod &pod::operator=(pod &&other) noexcept
    {
        m_impl = std::move(other.m_impl);
        return *this;
    }

    std::vector<void *> pod::array() const
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

    std::optional<pod_prop> pod::find(enum_value<prop> key) const
    {
        // Re-implementation of `spa_pod_find_prop` without const return value

        if (type() != spa::type::object)
        {
            return std::nullopt;
        }

        for (auto &&prop : as<pod_object>())
        {
            if (prop.key() != key)
            {
                continue;
            }

            return std::move(prop);
        }

        return std::nullopt;
    }

    std::optional<pod_prop> pod::find_recursive(enum_value<prop> key) const
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

    std::size_t pod::size() const
    {
        return m_impl->pod->size;
    }

    enum_value<spa::type> pod::type() const
    {
        return m_impl->pod->type;
    }

    template <>
    bool pod::as() const
    {
        assert(type() == spa::type::boolean);
        return reinterpret_cast<spa_pod_bool *>(m_impl->pod.get())->value;
    }

    template <>
    int pod::as() const
    {
        assert(type() == spa::type::num_int);
        return reinterpret_cast<spa_pod_int *>(m_impl->pod.get())->value;
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
    void pod::write(const bool &value)
    {
        assert(type() == spa::type::boolean);
        reinterpret_cast<spa_pod_bool *>(m_impl->pod.get())->value = value;
    }

    template <>
    void pod::write(const int &value)
    {
        assert(type() == spa::type::num_int);
        reinterpret_cast<spa_pod_int *>(m_impl->pod.get())->value = value;
    }

    template <>
    void pod::write(const float &value)
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
        return {view_deleter<raw_type>, pod};
    }

    pod pod::copy(const raw_type *pod)
    {
        static constexpr auto deleter = [](auto *pod) {
            free(pod); // NOLINT(*-malloc)
        };

        return {deleter, spa_pod_copy(pod)};
    }
} // namespace pipewire::spa
