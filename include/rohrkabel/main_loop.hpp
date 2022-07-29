#pragma once
#include <memory>

#include "utils/annotations.hpp"
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
        main_loop();
        ~main_loop();

      public:
        void quit() const;
        [[blocking]] void run() const;

      public:
        [[nodiscard]] pw_loop *get() const;
        [[nodiscard]] pw_main_loop *get_main_loop() const;
    };
} // namespace pipewire
#include "utils/annotations.hpp"