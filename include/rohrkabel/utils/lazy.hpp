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

    template <typename T, typename Function>
        requires std::same_as<std::invoke_result_t<Function>, T>
    lazy<T> make_lazy(Function &&func)
    {
        return std::async(std::launch::deferred, std::forward<Function>(func));
    }
} // namespace pipewire
