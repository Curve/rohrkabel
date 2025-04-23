#include "metadata/events.hpp"
#include "metadata/metadata.hpp"

#include <coco/promise/promise.hpp>

#include <pipewire/pipewire.h>
#include <pipewire/extensions/metadata.h>

namespace pipewire
{
    struct metadata::impl
    {
        raw_type *metadata;
        properties_t properties;
    };

    metadata::metadata(proxy &&base, properties_t properties) : proxy(std::move(base)), m_impl(std::make_unique<impl>())
    {
        m_impl->metadata   = reinterpret_cast<raw_type *>(proxy::get());
        m_impl->properties = std::move(properties);
    }

    metadata::metadata(metadata &&other) noexcept = default;

    metadata &metadata::operator=(metadata &&other) noexcept = default;

    metadata::~metadata() = default;

    void metadata::clear_property(std::uint32_t id, const std::string &key)
    {
        pw_metadata_set_property(m_impl->metadata, id, key.c_str(), nullptr, nullptr);
        m_impl->properties.erase(key);
    }

    void metadata::set_property(std::uint32_t id, std::string key, std::string type, std::string value)
    {
        pw_metadata_set_property(m_impl->metadata, id, key.c_str(), type.c_str(), value.c_str());
        m_impl->properties.emplace(std::move(key), metadata_property{std::move(type), std::move(value), id});
    }

    metadata::raw_type *metadata::get() const
    {
        return m_impl->metadata;
    }

    metadata::properties_t metadata::properties() const
    {
        return m_impl->properties;
    }

    metadata::operator raw_type *() const &
    {
        return get();
    }

    task<metadata> metadata::bind(raw_type *raw)
    {
        auto _proxy   = proxy::bind(reinterpret_cast<proxy::raw_type *>(raw));
        auto listener = metadata_listener{raw};

        auto properties = properties_t{};

        listener.on<metadata_event::property>([&](auto key, auto property) {
            properties.emplace(key, std::move(property));
            return 0;
        });

        co_await task<metadata>::wake_on_await{};
        auto proxy = co_await std::move(_proxy);

        if (!proxy.has_value())
        {
            co_return std::unexpected{proxy.error()};
        }

        co_return metadata{std::move(proxy.value()), std::move(properties)};
    }

    const char *metadata::type            = PW_TYPE_INTERFACE_Metadata;
    const std::uint32_t metadata::version = PW_VERSION_METADATA;
} // namespace pipewire
