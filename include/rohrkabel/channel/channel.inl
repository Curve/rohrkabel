#pragma once
#include "channel.hpp"

namespace pipewire
{
    template <typename T>
    sender<T>::sender(std::shared_ptr<cr::queue<T>> queue, std::shared_ptr<channel_state> state)
        : cr::sender<T>(queue), m_state(std::move(state))
    {
    }

    template <typename T>
    void sender<T>::send(T message)
    {
        cr::sender<T>::send(std::move(message));
        m_state->emit();
    }

    template <typename T>
    receiver<T>::receiver(std::shared_ptr<cr::queue<T>> queue, std::shared_ptr<channel_state> state)
        : cr::receiver<T>(queue), m_state(std::move(state))
    {
    }

    template <typename T>
    template <typename Callback>
        requires cr::visitable<T, Callback>
    void receiver<T>::attach(const std::shared_ptr<main_loop> &loop, Callback &&callback)
    {
        static auto receive = [this, callback = std::forward<Callback>(callback)]()
        {
            cr::receiver<T>::recv(callback);
        };

        m_state->attach(loop, std::move(receive));
    }

    template <typename... T>
    auto channel()
    {
        auto queue = std::make_shared<cr::queue<cr::internal::deduce_t<T...>>>();
        auto state = std::make_shared<channel_state>();

        return std::make_pair(sender{queue, state}, receiver{queue, state});
    }
} // namespace pipewire