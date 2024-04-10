#include "client/info.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    client_info client_info::from(const pw_client_info *info)
    {
        return {
            .props       = info->props,
            .id          = info->id,
            .change_mask = info->change_mask,
        };
    }
} // namespace pipewire
