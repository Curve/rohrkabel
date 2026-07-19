#include "module/info.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    module_info module_info::from(const pw_module_info *info)
    {
        return {
            .id          = info->id,
            .args        = info->args ? info->args : "",
            .filename    = info->filename ? info->filename : "",
            .props       = info->props,
            .name        = info->name ? info->name : "",
            .change_mask = info->change_mask,
        };
    }
} // namespace pipewire
