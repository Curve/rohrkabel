#pragma once
#include <map>
#include <string>

struct spa_dict;
namespace pipewire
{
    class dict : public std::map<std::string, std::string>
    {
      public:
        using map::map;
        dict(const spa_dict *);
    };
} // namespace pipewire