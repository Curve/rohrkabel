#pragma once
#include "loop.hpp"

#include <memory>

struct pw_loop;
struct pw_main_loop;
namespace pipewire
{
    class main_loop final : public loop
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        main_loop();
        ~main_loop() final;

      public:
        void run() const final;
        void quit() const final;

      public:
        [[nodiscard]] pw_loop *get() const final;
        [[nodiscard]] pw_main_loop *get_main_loop() const;
    };
} // namespace pipewire