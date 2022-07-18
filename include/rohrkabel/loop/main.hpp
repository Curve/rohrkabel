#pragma once
#include <functional>
#include <memory>
#include <queue>
#include <mutex>

struct pw_loop;
struct pw_main_loop;

namespace pipewire
{
    class main_loop
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        std::recursive_mutex m_queue_mutex;
        std::queue<std::function<void()>> m_queue;

      private:
        void emit_event() const;

      public:
        main_loop();
        ~main_loop();

      public:
        void run() const;
        void quit() const;

      public:
        [[nodiscard]] pw_loop *get() const;
        [[nodiscard]] pw_main_loop *get_main_loop() const;

      public:
        template <typename Function> [[nodiscard]] auto call_safe(Function &&function);
    };
} // namespace pipewire

#include "main.inl"