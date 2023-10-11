#include "utils/assert.hpp"

#include <stdexcept>

namespace pipewire
{
    void check(bool condition, const char *message)
    {
        if (condition)
        {
            return;
        }

        throw std::runtime_error(message);
    }
} // namespace pipewire
