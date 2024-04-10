#include "device/info.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    device_info device_info::from(const pw_device_info *info)
    {
        return {
            .props       = info->props,
            .id          = info->id,
            .change_mask = info->change_mask,
            .params      = spa::param_info::from(info->n_params, info->params),
        };
    }
} // namespace pipewire
