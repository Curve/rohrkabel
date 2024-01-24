#pragma once
#include "pod.hpp"

#include <string>
#include <memory>
#include <cstdint>

struct spa_pod_prop;
struct spa_type_info;

namespace pipewire::spa
{
    class pod_prop
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~pod_prop();

      private:
        pod_prop();

      public:
        pod_prop(pod_prop &&) noexcept;
        pod_prop(const pod_prop &) noexcept;

      public:
        pod_prop &operator=(pod_prop &&) noexcept;
        pod_prop &operator=(const pod_prop &) noexcept;

      public:
        [[nodiscard]] pod value() const;
        [[nodiscard]] std::string name() const;
        [[nodiscard]] std::uint32_t key() const;
        [[nodiscard]] std::uint32_t flags() const;

      public:
        [[nodiscard]] spa_pod_prop *get() const;
        [[nodiscard]] const spa_type_info *type_info() const;

      public:
        [[nodiscard]] static pod_prop view(spa_pod_prop *, const spa_type_info *);
    };
} // namespace pipewire::spa
