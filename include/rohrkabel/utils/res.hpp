#pragma once

#include "../error.hpp"

#include <expected>

namespace pipewire
{
    template <typename T = void, typename E = error>
    using res = std::expected<T, E>;
}
