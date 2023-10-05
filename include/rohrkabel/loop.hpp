#pragma once
#include <memory>

struct pw_loop;
struct pw_main_loop;

namespace pipewire
{
    class main_loop
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~main_loop();

      private:
        main_loop();

      public:
        void quit() const;
        [[rk::blocking]] void run() const;

      public:
        [[nodiscard]] pw_loop *loop() const;
        [[nodiscard]] pw_main_loop *get() const;

      public:
        [[nodiscard]] operator pw_main_loop *() const &;
        [[nodiscard]] operator pw_main_loop *() const && = delete;

      public:
        [[nodiscard]] static std::shared_ptr<main_loop> create();
    };
} // namespace pipewire