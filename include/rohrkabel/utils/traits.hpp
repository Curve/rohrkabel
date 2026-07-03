#pragma once

#include "task.hpp"

#include <vector>
#include <cstdint>
#include <concepts>

#include <coco/traits/traits.hpp>

namespace pipewire
{
    class proxy;

    namespace detail
    {
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
        using vector_type_t = vector_traits<T>::type;

        template <typename T>
        concept vector = vector_traits<T>::value;

        template <typename T, template <typename...> typename Cond, typename Traits = vector_traits<T>>
        concept vector_where = Traits::value && Cond<typename Traits::type>::value;

        template <typename T>
        concept pointer = std::is_pointer_v<T>;

        template <typename T, typename... Ts>
        concept one_of = (std::same_as<T, Ts> || ...);

        template <typename T, typename Raw>
        concept listener = std::constructible_from<T, Raw *>;

        template <typename T>
        concept proxy = requires(T obj) {
            requires std::derived_from<T, pipewire::proxy>;
            requires std::is_move_assignable_v<T>;
            requires std::is_move_constructible_v<T>;
            requires not std::is_pointer_v<typename T::raw_type>;
            { obj.get() } -> pointer;
            { obj.bind(nullptr) } -> std::same_as<task<T>>;
            { T::type } -> std::same_as<const char *&>;
            { T::version } -> std::same_as<const std::uint32_t &>;
        };

        template <typename T>
        concept factory = requires() { typename T::result; };

        template <typename T>
        concept awaitable = coco::awaitable<T>;

        template <awaitable T>
        using lazy_of = lazy<typename coco::traits<T>::result>;
    } // namespace detail
} // namespace pipewire
