#include "node/info.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    node_info node_info::from(const pw_node_info *info)
    {
        return {
            .id = info->id,
            .input =
                {
                    .max     = info->max_input_ports,
                    .current = info->n_input_ports,
                },
            .output =
                {
                    .max     = info->max_output_ports,
                    .current = info->n_output_ports,
                },
            .props       = info->props,
            .state       = static_cast<node_state>(info->state),
            .error       = info->error ? info->error : "",
            .change_mask = info->change_mask,
            .params      = spa::param_info::from(info->n_params, info->params),
        };
    }
} // namespace pipewire
