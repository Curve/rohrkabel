#pragma once
#include "../type_indentity.hpp"
#include <tuple>

namespace pipewire
{
    template <class Factory, typename... Params> struct factory
    {
        using factory_t = Factory;
        using args_t = std::tuple<Params...>;
    };

    template <typename> struct is_factory : std::false_type
    {
    };

    template <typename Factory, typename... Params> struct is_factory<factory<Factory, Params...>> : std::true_type
    {
    };

    template <class... Factories> class factory_handler
    {
        static_assert((is_factory<Factories>::value && ...));
        using tuple_t = std::tuple<Factories...>;

      public:
        template <class Factory, std::size_t I = 0> static constexpr auto get();
        template <class Factory> using get_t = typename decltype(get<Factory>())::type;
    };
} // namespace pipewire

#include "factories.inl"