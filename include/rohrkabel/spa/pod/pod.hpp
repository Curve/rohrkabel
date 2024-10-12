#pragma once

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
        pod(std::shared_ptr<raw_type>);

      public:
        pod(pod &&) noexcept;
        pod(const pod &) noexcept;

      public:
        pod &operator=(pod &&) noexcept;
        pod &operator=(const pod &) noexcept;

      public:
        [[nodiscard]] std::optional<pod_prop> find(prop) const;
        [[nodiscard]] std::optional<pod_prop> find_recursive(prop) const;

      public:
        [[nodiscard]] spa::type type() const;
        [[nodiscard]] std::size_t size() const;

      public:
        template <typename T>
        [[nodiscard]] T as() const = delete;

      public:
        template <typename T>
        void write(T) = delete;

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
    std::vector<void *> pod::as() const;

    template <>
    void pod::write(bool);
    template <>
    void pod::write(float);
} // namespace pipewire::spa
