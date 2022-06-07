#include "global.hpp"

namespace pipewire
{
    global::operator std::uint32_t() const
    {
        return id;
    }
} // namespace pipewire