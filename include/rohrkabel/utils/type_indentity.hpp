#pragma once

namespace pipewire
{
    template <typename T> struct type_identity
    {
        using type = T;
    };
} // namespace pipewire