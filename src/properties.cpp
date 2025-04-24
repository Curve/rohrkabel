#include "properties.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct properties::impl
    {
        pw_unique_ptr<raw_type> properties;
    };

    properties::properties(deleter<raw_type> deleter, raw_type *raw)
        : m_impl(std::make_unique<impl>(pw_unique_ptr<raw_type>{raw, deleter}))
    {
    }

    properties::properties(properties &&) noexcept = default;

    properties &properties::operator=(properties &&) noexcept = default;

    properties::~properties() = default;

    void properties::set(const std::string &key, const std::string &value)
    {
        pw_properties_set(m_impl->properties.get(), key.c_str(), value.c_str());
    }

    properties::mapped properties::map() const
    {
        auto rtn    = mapped{};
        void *state = nullptr;

        while (const auto *key = pw_properties_iterate(m_impl->properties.get(), &state))
        {
            rtn.emplace(key, get(key));
        }

        return rtn;
    }

    std::string properties::get(const std::string &key) const
    {
        return pw_properties_get(m_impl->properties.get(), key.c_str());
    }

    properties::raw_type *properties::get() const
    {
        return m_impl->properties.get();
    }

    properties properties::create()
    {
        return from(pw_properties_new(nullptr, nullptr));
    }

    properties properties::create(const mapped &map)
    {
        auto rtn = create();

        for (const auto &[key, value] : map)
        {
            rtn.set(key, value);
        }

        return rtn;
    }

    properties properties::from(raw_type *properties)
    {
        static constexpr auto deleter = [](auto *properties)
        {
            pw_properties_free(properties);
        };

        return {deleter, properties};
    }

    properties properties::view(raw_type *properties)
    {
        return {view_deleter<raw_type>, properties};
    }

    properties::operator raw_type *() const &
    {
        return get();
    }
} // namespace pipewire
