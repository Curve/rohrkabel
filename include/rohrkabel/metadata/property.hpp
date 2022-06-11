#pragma once
#include <string>
#include <cstdint>

namespace pipewire
{
    struct metadata_property
    {
        std::string type;
        std::string value;
        std::uint32_t subject;
    };
} // namespace pipewire