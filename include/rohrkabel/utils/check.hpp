#pragma once

#include <string_view>
#include <source_location>

namespace pipewire
{
    void check(bool, std::string_view, const std::source_location = std::source_location::current());
}
