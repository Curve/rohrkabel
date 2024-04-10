#include "core/info.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    core_info core_info::from(const pw_core_info *info)
    {
        return {
            .props     = info->props,
            .id        = info->id,
            .cookie    = info->cookie,
            .name      = info->name,
            .version   = info->version,
            .user_name = info->user_name,
            .host_name = info->host_name,
        };
    }
} // namespace pipewire
