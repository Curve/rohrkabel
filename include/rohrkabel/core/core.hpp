#pragma once

#include "../context.hpp"
#include "../properties.hpp"

#include "../utils/task.hpp"
#include "../utils/traits.hpp"
#include "../utils/deleter.hpp"

#include <memory>
#include <optional>

struct pw_core;

namespace pipewire
{
    class proxy;
    class core_listener;

    struct factory
    {
        using result = proxy;

      public:
        std::string name;
        properties props;

      public:
        std::optional<std::string> type;
        std::optional<std::uint32_t> version;
    };

    class core
    {
        struct impl;

      public:
        using raw_type = pw_core;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        core(deleter<raw_type>, raw_type *, std::shared_ptr<pipewire::context>);

      public:
        ~core();

      private:
        [[nodiscard]] void *create_object(factory) const;

      public:
        [[nodiscard]] int sync(int seq) const;
        [[nodiscard]] task<void> sync() const;

      public:
        void run_once() const;

      public:
        template <detail::Listener<raw_type> Listener = core_listener>
        [[nodiscard]] Listener listen() const;

      public:
        template <detail::Proxy T>
        [[nodiscard]] task<T> create(factory);

        template <typename Factory = factory>
        [[nodiscard]] task<typename Factory::result> create(Factory);

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
