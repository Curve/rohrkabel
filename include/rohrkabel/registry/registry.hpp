#pragma once

#include "../core/core.hpp"

#include <memory>
#include <cstdint>

struct pw_registry;

namespace pipewire
{
    class registry_listener;

    class registry
    {
        friend class core;

      public:
        using raw_type = pw_registry;

      private:
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        void *bind(std::uint32_t, const char *, std::uint32_t) const;

      public:
        ~registry();

      protected:
        registry();

      public:
        template <class Listener = registry_listener>
            requires valid_listener<Listener, raw_type>
        [[rk::needs_update]] [[nodiscard]] Listener listen();

      public:
        template <class T>
            requires valid_proxy<T>
        [[nodiscard]] lazy<expected<T>> bind(std::uint32_t id, update_strategy strategy = update_strategy::sync);

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] std::shared_ptr<pipewire::core> core() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      private:
        [[nodiscard]] static std::shared_ptr<registry> create(std::shared_ptr<pipewire::core>);
    };
} // namespace pipewire

#include "registry.inl"
