#pragma once
#include <map>
#include <string>

struct spa_dict;
namespace pipewire::spa
{
    class dict : public std::map<std::string, std::string>
    {
      public:
        using map::map;
        dict(const spa_dict *);
    };
} // namespace pipewire::spa