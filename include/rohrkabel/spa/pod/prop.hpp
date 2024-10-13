#pragma once

#include "pod.hpp"

#include <memory>
#include <cstdint>

struct spa_pod_prop;

namespace pipewire::spa
{
    class pod_prop
    {
        struct impl;

      public:
        using raw_type = spa_pod_prop;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~pod_prop();

      private:
        pod_prop(deleter<raw_type>, raw_type *);

      public:
        pod_prop(pod_prop &&) noexcept;

      public:
        pod_prop &operator=(pod_prop &&) noexcept;

      public:
        [[nodiscard]] pod value() const;
        [[nodiscard]] std::uint32_t key() const;
        [[nodiscard]] std::uint32_t flags() const;

      public:
        [[nodiscard]] raw_type *get() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[nodiscard]] static pod_prop view(raw_type *);
    };
} // namespace pipewire::spa
