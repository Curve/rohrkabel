#pragma once
#include "events.hpp"
#include "../core/core.hpp"

#include <memory>
#include <cstdint>

struct pw_registry;

namespace pipewire
{
    class registry
    {
        friend class core;

      private:
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        void *bind(std::uint32_t, const char *, std::uint32_t) const;

      public:
        ~registry();

      protected:
        registry(std::shared_ptr<core>);

      public:
        template <class Listener = registry_listener>
        [[rk::needs_update]] [[nodiscard]] Listener listen() = delete;

      public:
        template <class T>
            requires valid_proxy<T>
        [[nodiscard]] lazy<expected<T>> bind(std::uint32_t id, update_strategy strategy = update_strategy::sync);

      public:
        [[nodiscard]] pw_registry *get() const;
        [[nodiscard]] std::shared_ptr<core> core() const;

      public:
        [[nodiscard]] operator pw_registry *() const &;
        [[nodiscard]] operator pw_registry *() const && = delete;
    };

    template <>
    registry_listener registry::listen();
} // namespace pipewire

#include "registry.inl"