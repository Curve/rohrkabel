#include "listener.hpp"
#include "metadata/metadata.hpp"
#include "registry/registry.hpp"

#include <optional>
#include <pipewire/pipewire.h>
#include <pipewire/extensions/metadata.h>

namespace pipewire
{
    struct metadata::impl
    {
        pw_metadata *metadata;
        properties_t properties;
        pw_metadata_events events;
        std::optional<listener> hook;
    };

    metadata::~metadata()
    {
        if (m_impl)
        {
            pw_proxy_destroy(reinterpret_cast<pw_proxy *>(m_impl->metadata));
        }
    }

    metadata::metadata(metadata &&metadata) noexcept : m_impl(std::move(metadata.m_impl)) {}

    metadata::metadata(registry &registry, std::uint32_t id) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = PW_VERSION_METADATA_EVENTS;

        m_impl->events.property = [](void *data, std::uint32_t subject, const char *key, const char *type, const char *value) {
            auto &m_impl = *reinterpret_cast<impl *>(data);
            m_impl.properties.emplace(key, metadata_property{type, value, subject});

            return 0;
        };

        m_impl->hook.emplace();
        m_impl->metadata = reinterpret_cast<pw_metadata *>(pw_registry_bind(registry.get(), id, type.c_str(), version, sizeof(void *)));

        // NOLINTNEXTLINE
        pw_metadata_add_listener(m_impl->metadata, &m_impl->hook->get(), &m_impl->events, m_impl.get());
    }

    metadata &metadata::operator=(metadata &&metadata) noexcept
    {
        m_impl = std::move(metadata.m_impl);
        return *this;
    }

    metadata::properties_t metadata::properties() const
    {
        return m_impl->properties;
    }

    pw_metadata *metadata::get() const
    {
        return m_impl->metadata;
    }

    const std::string metadata::type = PW_TYPE_INTERFACE_Metadata;
    const std::uint32_t metadata::version = PW_VERSION_METADATA;
} // namespace pipewire