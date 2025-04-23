#include "channel/channel.hpp"
#include "loop.hpp"

#include <future>
#include <chrono>

#include <cstdint>

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct channel_state::impl
    {
        std::function<void()> callback;
        std::shared_ptr<main_loop> loop;

      public:
        std::promise<spa_source *> promise;
        std::shared_future<spa_source *> source;
    };

    channel_state::channel_state() : m_impl(std::make_unique<impl>())
    {
        m_impl->source = m_impl->promise.get_future().share();
    }

    channel_state::~channel_state()
    {
        if (!m_impl)
        {
            return;
        }

        if (!m_impl->source.valid())
        {
            return;
        }

        auto status = m_impl->source.wait_for(std::chrono::seconds(0));

        if (status != std::future_status::ready)
        {
            return;
        }

        auto *source = m_impl->source.get();

        if (source->fd < 0)
        {
            return;
        }

        pw_loop_destroy_source(m_impl->loop->loop(), source);
    }

    void channel_state::emit()
    {
        auto *source = m_impl->source.get();
        pw_loop_signal_event(m_impl->loop->loop(), source);
    }

    void channel_state::attach(std::shared_ptr<main_loop> loop, std::function<void()> callback)
    {
        auto receive = [](void *data, std::uint64_t) {
            reinterpret_cast<impl *>(data)->callback();
        };

        m_impl->callback = std::move(callback);
        m_impl->loop     = std::move(loop);

        auto *signal = pw_loop_add_event(m_impl->loop->loop(), receive, m_impl.get());
        m_impl->promise.set_value(signal);
    }
} // namespace pipewire
