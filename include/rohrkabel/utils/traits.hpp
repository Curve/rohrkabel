#pragma once

#include "task.hpp"

#include <vector>
#include <cstdint>
#include <concepts>

namespace pipewire
{
    class proxy;

    namespace detail
    {
        template <typename T>
        static constexpr bool sync_after_bind = false;

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
        concept Vector = vector_traits<T>::value;

        template <typename T, template <typename...> typename Cond, typename Traits = vector_traits<T>>
        concept VectorWhere = Traits::value && Cond<typename Traits::type>::value;

        template <typename T>
        concept Pointer = std::is_pointer_v<T>;

        template <typename T, typename... Ts>
        concept OneOf = (std::same_as<T, Ts> || ...);

        template <typename T, typename Raw>
        concept Listener = std::constructible_from<T, Raw *>;

        template <typename T>
        concept Proxy = requires(T obj) {
            requires std::derived_from<T, proxy>;
            requires std::is_move_assignable_v<T>;
            requires std::is_move_constructible_v<T>;
            requires not std::is_pointer_v<typename T::raw_type>;
            { obj.get() } -> Pointer;
            { obj.bind(nullptr) } -> std::same_as<task<T>>;
            { T::type } -> std::same_as<const char *&>;
            { T::version } -> std::same_as<const std::uint32_t &>;
        };
    } // namespace detail
} // namespace pipewire
