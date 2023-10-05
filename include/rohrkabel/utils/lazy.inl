#pragma once
#include "lazy.hpp"

namespace pipewire
{
    template <typename T, typename Function>
        requires std::same_as<std::invoke_result_t<Function>, T>
    lazy<T> make_lazy(Function &&func)
    {
        return std::async(std::launch::deferred, std::forward<Function>(func));
    }
} // namespace pipewire