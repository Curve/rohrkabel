#include "spa/param.hpp"

#include <pipewire/pipewire.h>

namespace pipewire::spa
{
    param_info param_info::from(spa_param_info info)
    {
        return {
            .id    = info.id,
            .flags = static_cast<param_flags>(info.flags),
        };
    }

    std::vector<param_info> param_info::from(std::uint32_t count, spa_param_info *info)
    {
        std::vector<param_info> rtn{count};

        for (auto i = 0u; info && count > i; ++i)
        {
            rtn.emplace_back(from(info[i]));
        }

        return rtn;
    }
} // namespace pipewire::spa
