#pragma once

#include <memory>

namespace pipewire
{
    template <typename T>
    using deleter = void (*)(T *);

    template <typename T>
    using pw_unique_ptr = std::unique_ptr<T, void (*)(T *)>;

    template <typename T>
    static constexpr inline auto view_deleter = [](T *) {
    };
} // namespace pipewire
