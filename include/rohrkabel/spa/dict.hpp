#pragma once

#include <map>
#include <string>

struct spa_dict;

namespace pipewire::spa
{
    struct dict : std::map<std::string, std::string>
    {
        using std::map<std::string, std::string>::map;

      public:
        dict(const spa_dict *);
    };
} // namespace pipewire::spa
