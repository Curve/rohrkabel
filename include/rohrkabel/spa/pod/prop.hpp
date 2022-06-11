#pragma once
#include <string>
#include <memory>
#include <cstdint>

struct spa_pod_prop;
struct spa_type_info;
namespace pipewire::spa
{
    class pod;
    class pod_prop
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~pod_prop();

      public:
        pod_prop(pod_prop &&) noexcept;
        pod_prop(spa_pod_prop *, const spa_type_info *);

      public:
        pod_prop &operator=(pod_prop &&) noexcept;

      public:
        pod value() const;
        std::string name() const;
        std::uint32_t key() const;
        std::uint32_t flags() const;

      public:
        spa_pod_prop *get() const;
        const spa_type_info *get_type() const;
    };
} // namespace pipewire::spa