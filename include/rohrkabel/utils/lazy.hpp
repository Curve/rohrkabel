#pragma once

#include "../error.hpp"

#include <future>
#include <tl/expected.hpp>

namespace pipewire
{
    template <typename T>
    using expected = tl::expected<T, error>;

    template <typename T>
    using lazy = std::future<T>;

    template <typename T, typename Function>
        requires std::same_as<std::invoke_result_t<Function>, T>
    lazy<T> make_lazy(Function &&);
} // namespace pipewire

#include "lazy.inl"
