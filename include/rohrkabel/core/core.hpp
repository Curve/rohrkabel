#pragma once

#include "../context.hpp"
#include "../properties.hpp"
#include "../utils/traits.hpp"
#include "../utils/deleter.hpp"

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

    class core_listener;

    class core
    {
        struct impl;

      public:
        using raw_type = pw_core;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~core();

      private:
        core(deleter<raw_type>, raw_type *, std::shared_ptr<pipewire::context>);

      private:
        [[nodiscard]] void *create(factory) const;

      public:
        template <update_strategy>
        cancellable_lazy<expected<bool>> update();

      public:
        cancellable_lazy<expected<bool>> update(update_strategy strategy = update_strategy::sync);

      public:
        [[nodiscard]] int sync(int seq);

      public:
        template <class Listener = core_listener>
            requires detail::valid_listener<Listener, raw_type>
        [[rk::needs_update]] [[nodiscard]] Listener listen();

      public:
        template <typename T>
            requires detail::valid_proxy<T>
        [[nodiscard]] lazy<expected<T>> create(factory, update_strategy strategy = update_strategy::sync);

        template <typename T, typename Factory = factory>
        [[nodiscard]] lazy<expected<T>> create(Factory, update_strategy strategy = update_strategy::sync);

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] std::shared_ptr<pipewire::context> context() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[nodiscard]] static std::shared_ptr<core> create(std::shared_ptr<pipewire::context>);

      public:
        [[nodiscard]] static std::shared_ptr<core> from(raw_type *, std::shared_ptr<pipewire::context>);
        [[nodiscard]] static std::shared_ptr<core> view(raw_type *, std::shared_ptr<pipewire::context>);

      public:
        static const char *type;
        static const std::uint32_t version;
        static const std::uint32_t core_id;
    };
} // namespace pipewire

#include "core.inl"
