#pragma once
#include <memory>

struct pw_loop;
namespace pipewire
{
    class loop
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        loop();
        virtual ~loop();

      public:
        virtual void run() const = 0;
        virtual void quit() const = 0;

      public:
        [[nodiscard]] virtual pw_loop *get() const = 0;
    };
} // namespace pipewire