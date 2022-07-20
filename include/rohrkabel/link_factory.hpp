#pragma once
#include "proxy.hpp"

#include <cstdint>

namespace pipewire
{
    class core;
    class link_factory : public proxy
    {
      public:
        link_factory(core &core, std::uint32_t input, std::uint32_t output);

      public:
        static const std::string type;
        static const std::uint32_t version;
    };
} // namespace pipewire