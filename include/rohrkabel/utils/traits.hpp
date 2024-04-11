#pragma once

#include "lazy.hpp"
#include "../proxy/proxy.hpp"

#include <cstdint>
#include <concepts>
#include <type_traits>

namespace pipewire
{
    template <typename T>
    concept is_pointer = std::is_pointer_v<T>;

    template <typename T>
    concept valid_proxy = requires(T obj) {
        requires std::derived_from<T, proxy>;
        requires std::is_nothrow_move_assignable_v<T>;
        requires std::is_nothrow_move_constructible_v<T>;
        requires not std::is_pointer_v<typename T::raw_type>;
        { obj.get() } -> is_pointer;
        { obj.bind(nullptr) } -> std::same_as<lazy<expected<T>>>;
        { obj.type } -> std::same_as<const char *&>;
        { obj.version } -> std::same_as<const std::uint32_t &>;
    };
} // namespace pipewire
