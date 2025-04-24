#pragma once

#include "pod.hpp"

#include <ranges>

namespace pipewire::spa
{
    template <detail::VectorWhere<std::is_arithmetic> T>
    T pod::as() const
    {
        using value_type = detail::vector_traits<T>::type;

        auto transform = [](auto &&x)
        {
            return *reinterpret_cast<value_type *>(x);
        };

        return array()                            //
               | std::views::transform(transform) //
               | std::ranges::to<std::vector>();
    }

    template <detail::VectorWhere<std::is_arithmetic> T>
    void pod::write(const T &value)
    {
        using value_type = detail::vector_traits<T>::type;
        auto raw         = array();

        for (auto i = 0uz; value.size() > i; i++)
        {
            *reinterpret_cast<value_type *>(raw[i]) = value[i];
        }
    }
} // namespace pipewire::spa
