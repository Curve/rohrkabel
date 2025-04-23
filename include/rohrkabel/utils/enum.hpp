#pragma once

#include <utility>
#include <type_traits>

namespace pipewire
{
    template <typename T>
        requires std::is_scoped_enum_v<T>
    struct enum_value
    {
        using underlying = std::underlying_type_t<T>;

      private:
        underlying m_value;

      public:
        enum_value(T value) : m_value(std::to_underlying(value)) {}
        enum_value(underlying value) : m_value(value) {}

      public:
        [[nodiscard]] T get() const
        {
            return static_cast<T>(m_value);
        }

        [[nodiscard]] underlying value() const
        {
            return m_value;
        }

      public:
        [[nodiscard]] operator T() const
        {
            return get();
        }

        [[nodiscard]] operator underlying() const
        {
            return value();
        }

      public:
        bool operator==(const enum_value &other) const
        {
            return value() == other.value();
        }

        bool operator==(const underlying &other) const
        {
            return value() == other;
        }

        bool operator==(const T &other) const
        {
            return get() == other;
        }
    };
} // namespace pipewire
