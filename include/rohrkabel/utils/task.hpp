#pragma once

#include "../error.hpp"

#include <expected>
#include <coco/task/task.hpp>

namespace pipewire
{
    template <typename T>
    using lazy = coco::task<T>;

    template <typename T>
    using expected = std::expected<T, error>;

    template <typename T>
    using task = coco::task<expected<T>>;
} // namespace pipewire
