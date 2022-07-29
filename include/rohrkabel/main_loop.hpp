#pragma once
#include <memory>

struct pw_loop;
struct pw_main_loop;
namespace pipewire
{
    class main_loop
    {
        friend class registry;
        friend class context;
        friend class core;
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        bool is_safe() const;

      public:
        main_loop();
        ~main_loop();

      public:
        void run() const;
        void quit() const;

      public:
        [[nodiscard]] pw_loop *get() const;
        [[nodiscard]] pw_main_loop *get_main_loop() const;
    };
} // namespace pipewire