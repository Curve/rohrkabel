#pragma once

#include "../../utils/enum.hpp"
#include "../../utils/traits.hpp"
#include "../../utils/deleter.hpp"

#include <string>
#include <memory>
#include <vector>

#include <optional>
#include <cstdint>

struct spa_pod;
struct spa_type_info;

namespace pipewire::spa
{
    class pod_prop;
    class pod_object;

    enum class type : std::uint8_t
    {
        string     = 8,
        boolean    = 2,
        object     = 15,
        num_int    = 4,
        num_long   = 5,
        num_float  = 6,
        num_double = 7,
        array      = 13,
    };

    enum class prop : std::uint32_t
    {
        mute            = 65540,
        channel_volumes = 65544,
    };

    template <typename T>
    concept PodReadable = detail::OneOf<T, bool, int, long, float, double, std::string, pod_object>;

    template <typename T>
    concept PodWritable = detail::OneOf<T, bool, int, long, float, double>;

    class pod
    {
        struct impl;

      public:
        using raw_type = spa_pod;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        pod(deleter<raw_type>, raw_type *);

      public:
        pod(pod &&) noexcept;
        pod &operator=(pod &&) noexcept;

      public:
        pod(const pod &);
        pod &operator=(const pod &);

      public:
        ~pod();

      private:
        [[nodiscard]] std::vector<void *> array() const;

      public:
        [[nodiscard]] std::optional<pod_prop> find(enum_value<prop>) const;
        [[nodiscard]] std::optional<pod_prop> find_recursive(enum_value<prop>) const;

      public:
        [[nodiscard]] std::size_t size() const;
        [[nodiscard]] enum_value<spa::type> type() const;

      public:
        template <PodReadable T>
        [[nodiscard]] T read() const;

        template <detail::VectorWhere<std::is_arithmetic> T>
        [[nodiscard]] T as() const;

      public:
        template <PodWritable T>
        void write(T);

        template <detail::VectorWhere<std::is_arithmetic> T>
        void write(const T &);

      public:
        [[nodiscard]] raw_type *get() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[nodiscard]] static pod view(raw_type *);
        [[nodiscard]] static pod copy(const raw_type *);
    };
} // namespace pipewire::spa

#include "pod.inl"
