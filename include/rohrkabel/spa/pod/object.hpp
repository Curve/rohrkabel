#pragma once

#include "pod.hpp"
#include "prop.hpp"

#include <memory>
#include <cstdint>

#include <coco/generator/generator.hpp>

struct spa_pod_object;

namespace pipewire::spa
{
    class pod_object
    {
        struct impl;

      public:
        using raw_type = spa_pod_object;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        pod_object(raw_type *);

      public:
        pod_object(pod_object &&) noexcept;
        pod_object &operator=(pod_object &&) noexcept;

      public:
        ~pod_object();

      public:
        [[nodiscard]] spa::pod pod() const &;
        [[nodiscard]] spa::pod pod() const && = delete;

      public:
        [[nodiscard]] spa::type type() const;
        [[nodiscard]] std::uint32_t id() const;

      public:
        [[nodiscard]] coco::generator<pod_prop> props() const;

      public:
        [[nodiscard]] raw_type *get() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[nodiscard]] static pod_object view(raw_type *);
    };
} // namespace pipewire::spa
