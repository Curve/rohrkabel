#pragma once

#include "../error.hpp"

#include <future>
#include <tl/expected.hpp>

namespace pipewire
{
    template <typename T>
    using expected = tl::expected<T, error>;

    template <typename T>
    struct lazy : std::future<T>
    {
        template <typename... Args>
        lazy(Args &&...args) : std::future<T>(std::forward<Args>(args)...)
        {
        }

        virtual ~lazy()
        {
            if (!this->valid())
            {
                return;
            }

            this->wait();
        }
    };

    template <typename T>
    class cancellable_lazy : public lazy<T>
    {
        std::stop_source m_stop;

      public:
        template <typename... Args>
        cancellable_lazy(std::stop_source stop, Args &&...args)
            : lazy<T>(std::forward<Args>(args)...), m_stop(std::move(stop))
        {
        }

        [[nodiscard]] std::stop_source stop_source() const
        {
            return m_stop;
        }
    };

    template <typename T, typename Function>
        requires std::same_as<std::invoke_result_t<Function>, T>
    lazy<T> make_lazy(Function &&func)
    {
        return std::async(std::launch::deferred, std::forward<Function>(func));
    }

    template <typename T, typename Function>
        requires std::same_as<std::invoke_result_t<Function, std::stop_token>, T>
    cancellable_lazy<T> make_cancellable_lazy(Function &&func)
    {
        auto source = std::stop_source{};
        auto fut    = std::async(std::launch::deferred, std::forward<Function>(func), source.get_token());

        return {source, std::move(fut)};
    }
} // namespace pipewire
