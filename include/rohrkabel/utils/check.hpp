#pragma once
#include <source_location>

namespace pipewire
{
    void check(bool, const char *, const std::source_location = std::source_location::current());
}
