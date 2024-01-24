#include "properties.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct deleter
    {
        void operator()(pw_properties *props)
        {
            pw_properties_free(props);
        }
    };

    struct properties::impl
    {
        underlying map;
        std::unique_ptr<pw_properties, deleter> properties;
    };

    properties::~properties() = default;

    properties::properties() : m_impl(std::make_unique<impl>())
    {
        m_impl->properties = std::unique_ptr<pw_properties, deleter>(pw_properties_new(nullptr, nullptr));
    }

    properties::properties(properties &&properties) noexcept : m_impl(std::move(properties.m_impl)) {}

    properties::properties(underlying map) : properties()
    {
        for (const auto &[key, value] : map)
        {
            pw_properties_set(m_impl->properties.get(), key.c_str(), value.c_str());
        }

        m_impl->map = std::move(map);
    }

    properties::properties(std::initializer_list<std::pair<const std::string, std::string>> init)
        : properties(underlying{init})
    {
    }

    void properties::set(std::string key, std::string value)
    {
        pw_properties_set(m_impl->properties.get(), key.c_str(), value.c_str());
        m_impl->map.emplace(std::move(key), std::move(value));
    }

    std::string properties::get(const std::string &value) const
    {
        return m_impl->map.at(value);
    }

    properties::underlying::const_iterator properties::end() const
    {
        return m_impl->map.end();
    }

    properties::underlying::const_iterator properties::begin() const
    {
        return m_impl->map.begin();
    }

    pw_properties *properties::get() const
    {
        return m_impl->properties.get();
    }

    properties::operator pw_properties *() const &
    {
        return get();
    }
} // namespace pipewire
