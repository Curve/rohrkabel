#pragma once
#include <map>
#include <string>

struct spa_dict;

namespace pipewire::spa
{
    struct dict : public std::map<std::string, std::string>
    {
        using map::map;
        dict(const spa_dict *);
    };
} // namespace pipewire::spa