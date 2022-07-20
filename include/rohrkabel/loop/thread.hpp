#pragma once
#include "loop.hpp"

#include <memory>
#include <string>

#include "../utils/annotations.hpp"
struct pw_loop;
struct pw_thread_loop;
namespace pipewire
{
    class thread_loop final : public loop
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~thread_loop() final;
        thread_loop(const std::string &name);

      public:
        void run() const final;
        void quit() const final;

      public:
        [[thread_safe]] void lock() const;
        [[thread_safe]] void unlock() const;

      public:
        [[nodiscard]] pw_loop *get() const final;
        [[nodiscard]] pw_thread_loop *get_thread_loop() const;
    };
} // namespace pipewire
#include "../utils/annotations.hpp"