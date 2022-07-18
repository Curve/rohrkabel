#include "core/core.hpp"
#include "link_factory.hpp"
#include <pipewire/pipewire.h>

namespace pipewire
{
    link_factory::link_factory(core &core, std::uint32_t input, std::uint32_t output)
        : proxy(core.create("link-factory", {{"link.input.port", std::to_string(input)}, {"link.output.port", std::to_string(output)}}, type, version))
    {
    }

    const std::string link_factory::type = PW_TYPE_INTERFACE_Link;
    const std::uint32_t link_factory::version = PW_VERSION_LINK;
} // namespace pipewire