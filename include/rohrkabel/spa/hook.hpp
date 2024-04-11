#pragma once

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

      public:
        hook();
        hook(hook &&) noexcept;

      public:
        hook &operator=(hook &&) noexcept;

      public:
        [[nodiscard]] raw_type *get() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;
    };
} // namespace pipewire::spa
