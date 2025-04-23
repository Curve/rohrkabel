#pragma once

#include "../core/core.hpp"
#include "../utils/traits.hpp"
#include "../utils/deleter.hpp"

#include <memory>
#include <cstdint>
#include <optional>

struct pw_registry;

namespace pipewire
{
    class registry_listener;

    class registry
    {
        struct impl;

      public:
        using raw_type = pw_registry;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        void *bind(std::uint32_t, const char *, std::uint32_t) const;

      private:
        registry(deleter<raw_type>, raw_type *, std::shared_ptr<pipewire::core>);

      public:
        registry(registry &&) noexcept;
        registry &operator=(registry &&) noexcept;

      public:
        ~registry();

      public:
        template <detail::Listener<raw_type> Listener = registry_listener>
        [[nodiscard]] Listener listen() const;

      public:
        template <detail::Proxy T>
        [[nodiscard]] task<T> bind(std::uint32_t id);

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] std::shared_ptr<pipewire::core> core() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[nodiscard]] static std::optional<registry> create(std::shared_ptr<pipewire::core>);

      public:
        [[nodiscard]] static registry from(raw_type *, std::shared_ptr<pipewire::core>);
        [[nodiscard]] static registry view(raw_type *, std::shared_ptr<pipewire::core>);

      public:
        static const char *type;
        static const std::uint32_t version;
    };
} // namespace pipewire

#include "registry.inl"
