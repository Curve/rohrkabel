#pragma once

#include "../utils/deleter.hpp"

#include <memory>

struct spa_hook;

namespace pipewire::spa
{
    class hook
    {
        struct impl;

      public:
        using raw_type = spa_hook;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~hook();

      private:
        hook(deleter<raw_type>, raw_type *);

      public:
        hook(hook &&) noexcept;

      public:
        hook &operator=(hook &&) noexcept;

      public:
        [[nodiscard]] raw_type *get() const;

      public:
        [[nodiscard]] static hook create();

      public:
        [[nodiscard]] static hook from(raw_type *);
        [[nodiscard]] static hook view(raw_type *);

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;
    };
} // namespace pipewire::spa
