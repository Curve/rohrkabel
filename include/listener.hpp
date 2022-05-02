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

      protected:
        [[nodiscard]] spa_hook &get();
    };
} // namespace pipewire