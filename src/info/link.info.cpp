#include "link/info.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    link_info link_info::from(const pw_link_info *info)
    {
        return {
            .props       = info->props,
            .state       = static_cast<link_state>(info->state),
            .id          = info->id,
            .change_mask = info->change_mask,
            .input =
                {
                    .port = info->input_port_id,
                    .node = info->input_node_id,
                },
            .output =
                {
                    .port = info->output_port_id,
                    .node = info->output_node_id,
                },
        };
    }
} // namespace pipewire
