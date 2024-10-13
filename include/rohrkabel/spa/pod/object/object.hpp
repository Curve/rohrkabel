#pragma once

#include "../pod.hpp"

#include <memory>
#include <cstdint>

struct spa_pod_object;

namespace pipewire::spa
{
    class pod_object
    {
        struct impl;

      public:
        using raw_type = spa_pod_object;

      public:
        class iterator;
        class sentinel;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~pod_object();

      private:
        pod_object(raw_type *);

      public:
        pod_object(pod_object &&) noexcept;

      public:
        pod_object &operator=(pod_object &&) noexcept;

      public:
        [[nodiscard]] spa::pod pod() const &;
        [[nodiscard]] spa::pod pod() const && = delete;

      public:
        [[nodiscard]] spa::type type() const;
        [[nodiscard]] std::uint32_t id() const;

      public:
        [[nodiscard]] sentinel end() const;
        [[nodiscard]] iterator begin() const;

      public:
        [[nodiscard]] raw_type *get() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[nodiscard]] static pod_object view(raw_type *);
    };
} // namespace pipewire::spa

#include "iterator.hpp"
