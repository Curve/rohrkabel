#pragma once
#include <functional>
#include <memory>
#include <queue>
#include <mutex>

#include "../utils/annotations.hpp"
struct pw_loop;
struct pw_main_loop;
namespace pipewire
{
    class proxy;
    class main_loop
    {
        template <typename, typename> friend class safe_proxy;
        friend class core;
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        std::recursive_mutex m_queue_mutex;
        std::queue<std::function<void()>> m_queue;

      private:
        std::recursive_mutex m_proxy_mutex;
        std::vector<std::shared_ptr<proxy>> m_proxies;

      protected:
        bool is_safe() const;

      protected:
        void emit_event() const;
        void emit_cleanup() const;

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
        template <typename Function> [[thread_safe]] [[nodiscard]] auto call_safe(Function &&function);
    };
} // namespace pipewire
#include "../utils/annotations.hpp"

#include "main.inl"