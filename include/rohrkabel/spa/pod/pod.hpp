#pragma once
#include <string>
#include <memory>
#include <cstdint>

struct spa_pod;
struct spa_type_info;
namespace pipewire::spa
{
    enum class pod_type
    {
        string = 8,
        boolean = 2,
        object = 15,
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

      public:
        pod(spa_pod *);
        pod(const spa_pod *);

      public:
        pod(pod &&) noexcept;

      public:
        pod &operator=(pod &&) noexcept;

      public:
        pod_type type() const;
        std::size_t size() const;
        std::string name() const;

      public:
        template <typename T> T as() const = delete;
        template <typename T> T body() const = delete;
        template <typename T> void write(const T &) = delete;

      public:
        spa_pod *get() const;
        const spa_type_info *get_type() const;
    };

    template <> bool pod::as() const;
    template <> float pod::as() const;
    template <> std::string pod::as() const;

    template <> pod_object_body pod::body<pod_object_body>() const;

    template <> void pod::write(const bool &);
    template <> void pod::write(const float &);
} // namespace pipewire::spa