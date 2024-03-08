#pragma once

#include <memory>

struct spa_hook;

namespace pipewire
{
    class listener
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~listener();

      public:
        listener();
        listener(listener &&) noexcept;

      public:
        listener &operator=(listener &&) noexcept;

      public:
        [[nodiscard]] spa_hook *get() const;

      public:
        [[nodiscard]] operator spa_hook *() const &;
        [[nodiscard]] operator spa_hook *() const && = delete;
    };
} // namespace pipewire
