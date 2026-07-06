#pragma once

#include "res.hpp"

#include <coco/task/task.hpp>

namespace pipewire
{
    template <typename T>
    using lazy = coco::task<T>;

    template <typename T>
    using task = coco::task<res<T>>;
} // namespace pipewire
