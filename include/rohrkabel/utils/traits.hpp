#pragma once

#include "lazy.hpp"
#include "../proxy/proxy.hpp"

#include <cstdint>
#include <concepts>
#include <type_traits>

namespace pipewire::detail
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
        { T::type } -> std::same_as<const char *&>;
        { T::version } -> std::same_as<const std::uint32_t &>;
    };

    template <typename T>
    struct vector_traits : std::false_type
    {
    };

    template <typename T>
    struct vector_traits<std::vector<T>> : std::true_type
    {
        using type = T;
    };

    template <typename T>
    concept is_vector = vector_traits<T>::value;

    template <typename T>
    using vector_type = vector_traits<T>::type;
} // namespace pipewire::detail
