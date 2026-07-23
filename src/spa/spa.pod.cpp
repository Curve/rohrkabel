#include "spa/pod/pod.hpp"

#include "spa/pod/prop.hpp"
#include "spa/pod/object.hpp"

#include <spa/debug/pod.h>
#include <spa/pod/builder.h>

#include <cassert>

namespace pipewire::spa
{
    std::uint32_t type::string       = SPA_TYPE_String;
    std::uint32_t type::boolean      = SPA_TYPE_Bool;
    std::uint32_t type::object       = SPA_TYPE_Object;
    std::uint32_t type::object_props = SPA_TYPE_OBJECT_Props;
    std::uint32_t type::num_int      = SPA_TYPE_Int;
    std::uint32_t type::num_long     = SPA_TYPE_Long;
    std::uint32_t type::num_float    = SPA_TYPE_Float;
    std::uint32_t type::num_double   = SPA_TYPE_Double;
    std::uint32_t type::array        = SPA_TYPE_Array;

    std::uint32_t prop::mute            = SPA_PROP_mute;
    std::uint32_t prop::monitor_mute    = SPA_PROP_monitorMute;
    std::uint32_t prop::volume          = SPA_PROP_volume;
    std::uint32_t prop::channel_volumes = SPA_PROP_channelVolumes;

    std::uint32_t param::props = SPA_PARAM_Props;

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
        assert("Converting pod to wrong type" && type() == spa::type::array);

        std::vector<void *> rtn;

        auto *const array = reinterpret_cast<spa_pod_array *>(m_impl->pod.get());
        void *iter        = {};

        SPA_POD_ARRAY_FOREACH(array, iter)
        {
            rtn.emplace_back(iter);
        }

        return rtn;
    }

    template <>
    pod_object pod::read() const;

    std::optional<pod_prop> pod::find(std::uint32_t prop) const
    {
        // Re-implementation of `spa_pod_find_prop` without const return value

        if (type() != spa::type::object)
        {
            return std::nullopt;
        }

        for (auto &&item : read<pod_object>().props())
        {
            if (item.key() != prop)
            {
                continue;
            }

            return std::move(item);
        }

        return std::nullopt;
    }

    std::optional<pod_prop> pod::find_recursive(std::uint32_t prop) const
    {
        return [prop](this auto &&impl, const auto &pod) -> std::optional<pod_prop> // NOLINT(*-recursion)
        {
            if (auto ret = pod.find(prop); ret)
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

    std::uint32_t pod::type() const
    {
        return m_impl->pod->type;
    }

    template <>
    bool pod::read() const
    {
        assert("Converting pod to wrong type" && type() == spa::type::boolean);
        return reinterpret_cast<spa_pod_bool *>(m_impl->pod.get())->value;
    }

    template <>
    int pod::read() const
    {
        assert("Converting pod to wrong type" && type() == spa::type::num_int);
        return reinterpret_cast<spa_pod_int *>(m_impl->pod.get())->value;
    }

    template <>
    long pod::read() const
    {
        assert("Converting pod to wrong type" && type() == spa::type::num_long);
        return reinterpret_cast<spa_pod_long *>(m_impl->pod.get())->value;
    }

    template <>
    float pod::read() const
    {
        assert("Converting pod to wrong type" && type() == spa::type::num_float);
        return reinterpret_cast<spa_pod_float *>(m_impl->pod.get())->value;
    }

    template <>
    double pod::read() const
    {
        assert("Converting pod to wrong type" && type() == spa::type::num_double);
        return reinterpret_cast<spa_pod_double *>(m_impl->pod.get())->value;
    }

    template <>
    pod_object pod::read() const
    {
        assert("Converting pod to wrong type" && type() == spa::type::object);
        return pod_object::view(reinterpret_cast<spa_pod_object *>(m_impl->pod.get()));
    }

    template <>
    std::string pod::read() const
    {
        assert("Converting pod to wrong type" && type() == spa::type::string);

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
        assert("Converting pod to wrong type" && type() == spa::type::boolean);
        reinterpret_cast<spa_pod_bool *>(m_impl->pod.get())->value = value;
    }

    template <>
    void pod::write(int value)
    {
        assert("Converting pod to wrong type" && type() == spa::type::num_int);
        reinterpret_cast<spa_pod_int *>(m_impl->pod.get())->value = value;
    }

    template <>
    void pod::write(long value)
    {
        assert("Converting pod to wrong type" && type() == spa::type::num_long);
        reinterpret_cast<spa_pod_long *>(m_impl->pod.get())->value = value;
    }

    template <>
    void pod::write(float value)
    {
        assert("Converting pod to wrong type" && type() == spa::type::num_float);
        reinterpret_cast<spa_pod_float *>(m_impl->pod.get())->value = value;
    }

    template <>
    void pod::write(double value)
    {
        assert("Converting pod to wrong type" && type() == spa::type::num_double);
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
