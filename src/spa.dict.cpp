#include "spa/dict.hpp"
#include <pipewire/pipewire.h>

namespace pipewire::spa
{
    dict::dict(const spa_dict *dict)
    {
        const spa_dict_item *item{};
        spa_dict_for_each(item, dict)
        {
            emplace(item->key, item->value);
        }
    }
} // namespace pipewire::spa