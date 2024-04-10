#pragma once

#include <memory>

struct spa_hook;

namespace pipewire::spa
{
    class hook
    {
        struct impl;

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
        [[nodiscard]] spa_hook *get() const;

      public:
        [[nodiscard]] operator spa_hook *() const &;
        [[nodiscard]] operator spa_hook *() const && = delete;
    };
} // namespace pipewire::spa
