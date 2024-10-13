#pragma once

#include "../../utils/enum.hpp"
#include "../../utils/traits.hpp"

#include <string>
#include <memory>
#include <vector>

#include <optional>
#include <cstdint>

struct spa_pod;
struct spa_type_info;

namespace pipewire::spa
{
    enum class type : std::uint8_t
    {
        string    = 8,
        boolean   = 2,
        object    = 15,
        num_float = 6,
        array     = 13,
    };

    enum class prop : std::uint32_t
    {
        mute            = 65540,
        channel_volumes = 65544,
    };

    class pod_prop;
    class pod_object;

    class pod
    {
        struct impl;

      public:
        using raw_type = spa_pod;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~pod();

      private:
        pod(deleter<raw_type>, raw_type *);

      public:
        pod(const pod &);
        pod(pod &&) noexcept;

      public:
        pod &operator=(const pod &);
        pod &operator=(pod &&) noexcept;

      private:
        [[nodiscard]] std::vector<void *> array() const;

      public:
        [[nodiscard]] std::optional<pod_prop> find(enum_value<prop>) const;
        [[nodiscard]] std::optional<pod_prop> find_recursive(enum_value<prop>) const;

      public:
        [[nodiscard]] std::size_t size() const;
        [[nodiscard]] enum_value<spa::type> type() const;

      public:
        template <typename T>
        [[nodiscard]] T as() const = delete;

        template <typename T>
            requires(detail::is_vector<T> and std::is_arithmetic_v<detail::vector_type<T>>)
        [[nodiscard]] T as() const;

      public:
        template <typename T>
        void write(const T &) = delete;

        template <typename T>
            requires(detail::is_vector<T> and std::is_arithmetic_v<detail::vector_type<T>>)
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

    template <>
    bool pod::as() const;
    template <>
    float pod::as() const;
    template <>
    pod_object pod::as() const;
    template <>
    std::string pod::as() const;

    template <>
    void pod::write(const bool &);
    template <>
    void pod::write(const float &);
} // namespace pipewire::spa

#include "pod.inl"
