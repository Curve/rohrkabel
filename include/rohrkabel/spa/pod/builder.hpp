#pragma once

#include "pod.hpp"
#include "../../utils/deleter.hpp"

#include <memory>
#include <cstdint>
#include <optional>

struct spa_pod_builder;

namespace pipewire::spa
{
    class pod_builder
    {
        struct impl;

      public:
        using raw_type = spa_pod_builder;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        pod_builder(deleter<raw_type>, raw_type *);

      public:
        pod_builder(pod_builder &&) noexcept;
        pod_builder &operator=(pod_builder &&) noexcept;

      public:
        ~pod_builder();

      public:
        template <PodWritable T>
        void write(T);
        void prop(std::uint32_t prop, std::uint32_t flags = 0);

      public:
        void push_object(std::uint32_t type, std::uint32_t id);
        [[nodiscard]] std::optional<pod> pop();

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] raw_type *release() &&;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[nodiscard]] static pod_builder create();
        [[nodiscard]] static pod_builder view(raw_type *);
    };
} // namespace pipewire::spa
