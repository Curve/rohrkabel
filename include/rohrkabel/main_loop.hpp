#pragma once
#include <memory>

struct pw_main_loop;
namespace pipewire
{
    class main_loop
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        main_loop();
        ~main_loop();

      public:
        void run() const;
        void quit() const;

      public:
        [[nodiscard]] pw_main_loop *get() const;
    };
} // namespace pipewire