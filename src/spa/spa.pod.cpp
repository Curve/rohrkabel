#include "spa/pod/pod.hpp"

#include "spa/pod/prop.hpp"
#include "spa/pod/object.hpp"

#include "utils/check.hpp"

#include <spa/debug/pod.h>
#include <spa/pod/builder.h>

namespace pipewire::spa
{
    struct pod::impl
    {
        pw_unique_ptr<raw_type> pod;
    };

    pod::pod(deleter<raw_type> deleter, raw_type *pod) : m_impl(std::make_unique<impl>(pw_unique_ptr<raw_type>{pod, deleter})) {}

    pod::pod(pod &&) noexcept = default;

    pod &pod::operator=(pod &&) noexcept = default;

    pod::pod(const pod &other) : m_impl(std::move(copy(other.get()).m_impl)) {}

    pod &pod::operator=(const pod &other)
    {
        if (this != &other)
        {
            m_impl = std::move(copy(other.get()).m_impl);
        }

        return *this;
    }

    pod::~pod() = default;

    std::vector<void *> pod::array() const
    {
        check(type() == spa::type::array, "Converting pod to wrong type");

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
    pod_object pod::read() const;

    std::optional<pod_prop> pod::find(enum_value<prop> key) const
    {
        // Re-implementation of `spa_pod_find_prop` without const return value

        if (type() != spa::type::object)
        {
            return std::nullopt;
        }

        for (auto &&prop : read<pod_object>().props())
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
        return [key](this auto &&impl, const auto &pod) -> std::optional<pod_prop> // NOLINT(*-recursion)
        {
            if (auto ret = pod.find(key); ret)
            {
                return ret;
            }

            if (pod.type() != spa::type::object)
            {
                return std::nullopt;
            }

            for (const auto &child : pod.template read<spa::pod_object>().props())
            {
                auto ret = impl(child.value());

                if (!ret)
                {
                    continue;
                }

                return ret;
            }

            return std::nullopt;
        }(*this);
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
    bool pod::read() const
    {
        check(type() == spa::type::boolean, "Converting pod to wrong type");
        return reinterpret_cast<spa_pod_bool *>(m_impl->pod.get())->value;
    }

    template <>
    int pod::read() const
    {
        check(type() == spa::type::num_int, "Converting pod to wrong type");
        return reinterpret_cast<spa_pod_int *>(m_impl->pod.get())->value;
    }

    template <>
    long pod::read() const
    {
        check(type() == spa::type::num_long, "Converting pod to wrong type");
        return reinterpret_cast<spa_pod_long *>(m_impl->pod.get())->value;
    }

    template <>
    float pod::read() const
    {
        check(type() == spa::type::num_float, "Converting pod to wrong type");
        return reinterpret_cast<spa_pod_float *>(m_impl->pod.get())->value;
    }

    template <>
    double pod::read() const
    {
        check(type() == spa::type::num_double, "Converting pod to wrong type");
        return reinterpret_cast<spa_pod_double *>(m_impl->pod.get())->value;
    }

    template <>
    pod_object pod::read() const
    {
        check(type() == spa::type::object, "Converting pod to wrong type");
        return pod_object::view(reinterpret_cast<spa_pod_object *>(m_impl->pod.get()));
    }

    template <>
    std::string pod::read() const
    {
        check(type() == spa::type::string, "Converting pod to wrong type");

        const auto *content = SPA_POD_CONTENTS(spa_pod_string, m_impl->pod.get());

        if (!content)
        {
            return {};
        }

        return {reinterpret_cast<const char *>(content)};
    }

    template <>
    void pod::write(bool value)
    {
        check(type() == spa::type::boolean, "Converting pod to wrong type");
        reinterpret_cast<spa_pod_bool *>(m_impl->pod.get())->value = value;
    }

    template <>
    void pod::write(int value)
    {
        check(type() == spa::type::num_int, "Converting pod to wrong type");
        reinterpret_cast<spa_pod_int *>(m_impl->pod.get())->value = value;
    }

    template <>
    void pod::write(long value)
    {
        check(type() == spa::type::num_long, "Converting pod to wrong type");
        reinterpret_cast<spa_pod_long *>(m_impl->pod.get())->value = value;
    }

    template <>
    void pod::write(float value)
    {
        check(type() == spa::type::num_float, "Converting pod to wrong type");
        reinterpret_cast<spa_pod_float *>(m_impl->pod.get())->value = value;
    }

    template <>
    void pod::write(double value)
    {
        check(type() == spa::type::num_double, "Converting pod to wrong type");
        reinterpret_cast<spa_pod_double *>(m_impl->pod.get())->value = value;
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
        static constexpr auto deleter = [](auto *pod)
        {
            free(pod); // NOLINT(*-malloc)
        };

        return {deleter, spa_pod_copy(pod)};
    }
} // namespace pipewire::spa
