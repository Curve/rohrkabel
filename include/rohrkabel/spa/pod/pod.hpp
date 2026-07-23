#pragma once

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

    struct type
    {
        static std::uint32_t string;
        static std::uint32_t boolean;
        static std::uint32_t object;
        static std::uint32_t object_props;
        static std::uint32_t num_int;
        static std::uint32_t num_long;
        static std::uint32_t num_float;
        static std::uint32_t num_double;
        static std::uint32_t array;
    }; // namespace type

    struct prop
    {
        static std::uint32_t mute;
        static std::uint32_t volume;
        static std::uint32_t channel_volumes;
    }; // namespace prop

    struct param
    {
        static std::uint32_t props;
    }; // namespace prop

    template <typename T>
    concept PodReadable = detail::one_of<T, bool, int, long, float, double, std::string, pod_object>;

    template <typename T>
    concept PodWritable = detail::one_of<T, bool, int, long, float, double>;

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
        [[nodiscard]] std::optional<pod_prop> find(std::uint32_t prop) const;
        [[nodiscard]] std::optional<pod_prop> find_recursive(std::uint32_t prop) const;

      public:
        [[nodiscard]] std::size_t size() const;
        [[nodiscard]] std::uint32_t type() const;

      public:
        template <PodReadable T>
        [[nodiscard]] T read() const;

        template <detail::vector_where<std::is_arithmetic> T>
        [[nodiscard]] T read() const;

      public:
        template <PodWritable T>
        void write(T);

        template <detail::vector_where<std::is_arithmetic> T>
        void write(const T &);

      public:
        [[nodiscard]] raw_type *get() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[nodiscard]] static pod view(raw_type *);
        [[nodiscard]] static pod copy(const raw_type *);

      public:
        [[nodiscard]] static pod create();
    };
} // namespace pipewire::spa

#include "pod.inl"
