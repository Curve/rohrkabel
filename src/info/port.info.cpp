#include "port/info.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    port_info port_info::from(const pw_port_info *info)
    {
        return {
            .id          = info->id,
            .direction   = static_cast<port_direction>(info->direction),
            .change_mask = info->change_mask,
            .props       = info->props,
            .params      = spa::param_info::from(info->n_params, info->params),
        };
    }
} // namespace pipewire
