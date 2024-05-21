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

      public:
        using raw_type = spa_pod_prop;
        using raw_info = spa_type_info;

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
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] const raw_info *type_info() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[nodiscard]] static pod_prop view(raw_type *, const raw_info *);
    };
} // namespace pipewire::spa
