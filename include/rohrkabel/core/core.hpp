#pragma once
#include "events.hpp"
#include "../context.hpp"
#include "../properties.hpp"
#include "../utils/lazy.hpp"

#include <memory>
#include <optional>

struct pw_core;

namespace pipewire
{
    enum class update_strategy : std::uint8_t
    {
        sync,
        none,
    };

    struct factory
    {
        std::string name;
        properties props;

      public:
        std::optional<std::string> type;
        std::optional<std::uint32_t> version;
    };

    class registry;

    class core : public std::enable_shared_from_this<core>
    {
        friend class context;

      private:
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~core();

      private:
        core(std::shared_ptr<pipewire::context>);

      public:
        template <update_strategy>
        void update();

      public:
        void update(update_strategy strategy = update_strategy::sync);

      public:
        [[nodiscard]] int sync(int seq);

      public:
        template <class Listener = core_listener>
        [[rk::needs_update]] [[nodiscard]] Listener listen() = delete;

        template <typename T, typename Factory = factory>
        [[nodiscard]] lazy<expected<T>> create(const Factory &, update_strategy strategy = update_strategy::sync);

      public:
        [[nodiscard]] std::shared_ptr<registry> registry();

      public:
        [[nodiscard]] pw_core *get() const;
        [[nodiscard]] std::shared_ptr<context> context() const;

      public:
        [[nodiscard]] operator pw_core *() const &;
        [[nodiscard]] operator pw_core *() const && = delete;
    };

    template <>
    core_listener core::listen();
} // namespace pipewire

#include "core.inl"