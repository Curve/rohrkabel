#include "properties.hpp"
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct properties::impl
    {
        map_t map;
        pw_properties *properties;
    };

    properties::~properties()
    {
        if (m_impl)
        {
            pw_properties_free(m_impl->properties);
        }
    }

    properties::properties() : m_impl(std::make_unique<impl>())
    {
        m_impl->properties = pw_properties_new(nullptr, nullptr);
    }

    properties::properties(map_t &&map) : m_impl(std::make_unique<impl>())
    {
        m_impl->map = std::move(map);
        m_impl->properties = pw_properties_new(nullptr, nullptr);

        for (const auto &[key, value] : m_impl->map)
        {
            pw_properties_set(m_impl->properties, key.c_str(), value.c_str());
        }
    }

    properties::properties(const map_t &map) : m_impl(std::make_unique<impl>())
    {
        m_impl->map = map;
        m_impl->properties = pw_properties_new(nullptr, nullptr);

        for (const auto &[key, value] : m_impl->map)
        {
            pw_properties_set(m_impl->properties, key.c_str(), value.c_str());
        }
    }

    properties::properties(std::initializer_list<std::pair<const std::string, std::string>> &&initializer_list) : properties(std::map<std::string, std::string>(initializer_list))
    {
    }

    properties::properties(properties &&properties) noexcept : m_impl(std::move(properties.m_impl)) {}

    void properties::set(const std::string &key, const std::string &value)
    {
        m_impl->map.emplace(key, value);
        pw_properties_set(m_impl->properties, key.c_str(), value.c_str());
    }

    std::string properties::get(const std::string &value) const
    {
        return m_impl->map.at(value);
    }

    pw_properties *properties::get() const
    {
        return m_impl->properties;
    }

    properties::map_t::const_iterator properties::end() const
    {
        return m_impl->map.end();
    }

    properties::map_t::const_iterator properties::begin() const
    {
        return m_impl->map.begin();
    }
} // namespace pipewire