#pragma once
#include "../pod.hpp"
#include "iterator.hpp"

#include <memory>
#include <cstdint>

struct spa_type_info;
struct spa_pod_object_body;

namespace pipewire::spa
{
    class pod_object_body
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~pod_object_body();

      public:
        pod_object_body(const pod &);
        pod_object_body(pod_object_body &&) noexcept;

      public:
        pod_object_body &operator=(pod_object_body &&) noexcept;

      public:
        bool has(std::uint32_t key) const;
        pod_prop at(std::uint32_t key) const;

      public:
        pod_object_body_iterator end() const;
        pod_object_body_iterator begin() const;

      public:
        pod_type type() const;
        std::uint32_t id() const;

      public:
        spa_pod_object_body *get() const;
        const spa_type_info *get_type() const;
    };
} // namespace pipewire::spa