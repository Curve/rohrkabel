#pragma once
#include "channel.hpp"

namespace pipewire
{
    template <typename... Messages> sender<Messages...>::sender(sender &&other) noexcept = default;

    template <typename... Messages> //
    sender<Messages...>::sender(std::shared_ptr<channel_state> state, decltype(m_mutex) mutex, decltype(m_queue) queue)
        : sender_impl(state), m_mutex(std::move(mutex)), m_queue(std::move(queue))
    {
    }

    template <typename... Messages> //
    template <typename T>
    void sender<Messages...>::send(T &&message)
    {
        {
            std::lock_guard guard(*m_mutex);
            m_queue->emplace(std::forward<T>(message));
        }
        emit_signal();
    }

    template <typename... Messages> receiver<Messages...>::receiver(receiver &&other) noexcept = default;

    template <typename... Messages> //
    receiver<Messages...>::receiver(std::shared_ptr<channel_state> state, decltype(m_mutex) mutex, decltype(m_queue) queue)
        : receiver_impl(state), m_mutex(std::move(mutex)), m_queue(std::move(queue))
    {
    }

    template <typename... Messages> //
    template <typename Callback>
    void receiver<Messages...>::attach(main_loop *loop, Callback &&callback)
    {
        receiver_impl::attach(loop);

        on_receive = [this, callback = std::forward<Callback>(callback)]() {
            std::lock_guard guard(*m_mutex);

            while (!m_queue->empty())
            {
                auto entry = std::move(m_queue->front());
                m_queue->pop();

                std::visit(callback, std::move(entry));
            }
        };
    }

    template <typename... Messages> std::pair<sender<Messages...>, receiver<Messages...>> channel()
    {
        auto state = make_state();
        auto mutex = std::make_shared<std::mutex>();
        auto queue = std::make_shared<std::queue<std::variant<Messages...>>>();

        return {sender<Messages...>{state, mutex, queue}, receiver<Messages...>{state, mutex, queue}};
    }
} // namespace pipewire
