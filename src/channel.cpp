#include "loop.hpp"
#include "channel/channel.hpp"

#include <future>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct channel_state
    {
        main_loop *loop;
        spa_source *signal;

      public:
        std::promise<void> attached;
        std::future<void> ready = attached.get_future();
    };

    void sender_impl::emit_signal()
    {
        m_state->ready.wait();

        pw_loop_signal_event(m_state->loop->loop(), m_state->signal);
    }

    sender_impl::~sender_impl() = default;

    sender_impl::sender_impl(sender_impl &&) noexcept = default;

    sender_impl::sender_impl(std::shared_ptr<channel_state> state) : m_state(std::move(state)) {}

    void receiver_impl::attach(main_loop *loop)
    {
        m_state->loop = loop;

        // NOLINTNEXTLINE
        m_state->signal = pw_loop_add_event(
            loop->loop(),
            [](void *data, std::uint64_t)
            {
                auto &thiz = *reinterpret_cast<receiver_impl *>(data);
                thiz.on_receive();
            },
            this);

        m_state->attached.set_value();
    }

    receiver_impl::~receiver_impl()
    {
        // ? Depending on the scenario, the main_loop may've already removed the source itself, thus we check the fd
        if (!m_state || !m_state->ready.valid() || m_state->signal->fd < 0)
        {
            return;
        }

        pw_loop_remove_source(m_state->loop->loop(), m_state->signal);
    }

    receiver_impl::receiver_impl(receiver_impl &&) noexcept = default;

    receiver_impl::receiver_impl(std::shared_ptr<channel_state> state) : m_state(std::move(state)) {}

    std::shared_ptr<channel_state> make_state()
    {
        return std::make_shared<channel_state>();
    }
} // namespace pipewire