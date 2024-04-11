#pragma once

#include <memory>

struct pw_loop;
struct pw_main_loop;

namespace pipewire
{
    class main_loop
    {
        struct impl;

      public:
        using raw_type = pw_main_loop;

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
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] pw_loop *loop() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[nodiscard]] static std::shared_ptr<main_loop> create();
    };
} // namespace pipewire
