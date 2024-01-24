#pragma once
#include <string>
#include <memory>
#include <cstdint>

struct spa_pod;
struct spa_type_info;

namespace pipewire::spa
{
    enum class pod_type : std::uint8_t
    {
        string    = 8,
        boolean   = 2,
        object    = 15,
        num_float = 6,
    };

    class pod_object_body;

    class pod
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~pod();

      private:
        pod(std::shared_ptr<spa_pod>);

      public:
        pod(pod &&) noexcept;
        pod(const pod &) noexcept;

      public:
        pod &operator=(pod &&) noexcept;
        pod &operator=(const pod &) noexcept;

      public:
        [[nodiscard]] pod_type type() const;
        [[nodiscard]] std::size_t size() const;
        [[nodiscard]] std::string name() const;

      public:
        template <typename T>
        [[nodiscard]] T body() const = delete;

      public:
        template <typename T>
        [[nodiscard]] T read() const = delete;

      public:
        template <typename T>
        void write(T) = delete;

      public:
        [[nodiscard]] spa_pod *get() const;
        [[nodiscard]] const spa_type_info *type_info() const;

      public:
        [[nodiscard]] operator spa_pod *() const &;
        [[nodiscard]] operator spa_pod *() const && = delete;

      public:
        [[nodiscard]] static pod view(spa_pod *);
        [[nodiscard]] static pod copy(const spa_pod *);
    };

    template <>
    bool pod::read() const;
    template <>
    float pod::read() const;
    template <>
    std::string pod::read() const;

    template <>
    pod_object_body pod::body<pod_object_body>() const;

    template <>
    void pod::write(bool);
    template <>
    void pod::write(float);
} // namespace pipewire::spa
