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

      private:
        void destroy();

      public:
        ~listener();

      public:
        listener();
        listener(listener &&) noexcept;

      public:
        listener &operator=(listener &&) noexcept;

      public:
        [[nodiscard]] spa_hook &get();
    };
} // namespace pipewire