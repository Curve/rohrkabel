#pragma once

#include "../context.hpp"
#include "../properties.hpp"

#include "../utils/task.hpp"
#include "../utils/traits.hpp"
#include "../utils/deleter.hpp"

#include <cstdint>
#include <system_error>

#include <memory>
#include <optional>

struct pw_core;

namespace pipewire
{
    class proxy;
    class core_listener;

    struct object
    {
        using result = proxy;

      public:
        std::string name;
        properties props;

      public:
        std::optional<std::string> type;
        std::optional<std::uint32_t> version;
    };

    enum class sync_mode : std::uint8_t
    {
        basic,
        recursive,
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
        [[nodiscard]] void *create_object(object) const;

      public:
        [[nodiscard]] int sync(int seq) const;

      public:
        template <sync_mode = sync_mode::basic>
        [[nodiscard]] lazy<int> sync() const;

      public:
        template <detail::proxy T>
        [[nodiscard]] task<T> create(object);

        template <detail::factory Factory = object>
        [[nodiscard]] task<typename Factory::result> create(Factory);

      public:
        void run_once() const;

      public:
        template <detail::awaitable Awaitable>
        auto wait(Awaitable) const;

        template <detail::awaitable Awaitable>
        detail::lazy_of<Awaitable> await(Awaitable) const;

      public:
        [[nodiscard]] std::shared_ptr<pipewire::context> context() const;

      public:
        template <detail::listener<raw_type> Listener = core_listener>
        [[nodiscard]] Listener listen() const;

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] raw_type *release() &&;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[nodiscard]] static res<std::shared_ptr<core>, std::error_code> create(std::shared_ptr<pipewire::context>);

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
