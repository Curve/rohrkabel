#pragma once

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
        ~channel_state();

      public:
        channel_state();

      public:
        void emit();

      public:
        void attach(std::shared_ptr<main_loop>, std::function<void()> callback);
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
        template <typename Callback>
            requires cr::visitable<T, Callback>
        void attach(const std::shared_ptr<main_loop> &, Callback &&);
    };

    template <typename... T>
    struct recipe
    {
        using is_recipe = std::true_type;
        using sender    = pipewire::sender<cr::internal::deduce_t<T...>>;
        using receiver  = pipewire::receiver<cr::internal::deduce_t<T...>>;
    };

    template <typename... T>
    auto channel();
} // namespace pipewire

#include "channel.inl"
