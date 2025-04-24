#pragma once

#include <memory>
#include <functional>

#include <cr/channel.hpp>

struct spa_source;

namespace pipewire
{
    class main_loop;

    class channel_state
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        channel_state();

      public:
        ~channel_state();

      public:
        void emit();

      public:
        void attach(std::shared_ptr<main_loop>, std::move_only_function<void()> callback);
    };

    template <typename T>
    class sender : private cr::sender<T>
    {
        std::shared_ptr<channel_state> m_state;

      public:
        sender(std::shared_ptr<cr::queue<T>>, std::shared_ptr<channel_state>);

      public:
        void send(T = {});
    };

    template <typename T>
    class receiver : private cr::receiver<T>
    {
        std::shared_ptr<channel_state> m_state;

      public:
        receiver(std::shared_ptr<cr::queue<T>>, std::shared_ptr<channel_state>);

      public:
        template <cr::Visitable<T> Callback>
        void attach(std::shared_ptr<main_loop>, Callback &&);
    };

    template <typename... T>
    struct recipe
    {
        using sender   = pipewire::sender<cr::impl::deduce_t<T...>>;
        using receiver = pipewire::receiver<cr::impl::deduce_t<T...>>;

      public:
        static constexpr bool is_recipe = true;
    };

    template <typename... T>
    auto channel();
} // namespace pipewire

#include "channel.inl"
