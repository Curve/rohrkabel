#pragma once

#include "pod.hpp"

namespace pipewire::spa
{
    template <typename T>
        requires(detail::is_vector<T> and std::is_arithmetic_v<detail::vector_type<T>>)
    T pod::as() const
    {
        auto rtn = array() | std::views::transform([](auto &&x) {
                       return *reinterpret_cast<detail::vector_type<T> *>(x);
                   });

        return {rtn.begin(), rtn.end()};
    }

    template <typename T>
        requires(detail::is_vector<T> and std::is_arithmetic_v<detail::vector_type<T>>)
    void pod::write(const T &value)
    {
        auto raw = array();

        for (auto i = 0u; raw.size() > i; ++i)
        {
            if (raw.size() <= i)
            {
                continue;
            }

            *reinterpret_cast<detail::vector_type<T> *>(raw[i]) = value[i];
        }
    }
} // namespace pipewire::spa
