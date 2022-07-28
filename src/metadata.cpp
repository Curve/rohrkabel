#include "listener.hpp"
#include "metadata/metadata.hpp"

#include <pipewire/pipewire.h>
#include <pipewire/extensions/metadata.h>

namespace pipewire
{
    struct metadata::impl
    {
        pw_metadata *metadata;
        std::map<const std::string, const metadata_property> properties;
    };

    metadata::~metadata() = default;

    metadata::metadata(metadata &&metadata) noexcept : proxy(std::move(metadata)), m_impl(std::move(metadata.m_impl)) {}

    metadata::metadata(proxy &&_proxy, std::map<const std::string, const metadata_property> properties) : proxy(std::move(_proxy)), m_impl(std::make_unique<impl>())
    {
        m_impl->properties = std::move(properties);
        m_impl->metadata = reinterpret_cast<pw_metadata *>(proxy::get());
    }

    metadata &metadata::operator=(metadata &&metadata) noexcept
    {
        proxy::operator=(std::move(metadata));
        m_impl = std::move(metadata.m_impl);
        return *this;
    }

    lazy_expected<metadata> metadata::bind(pw_metadata *raw_metadata)
    {
        struct state
        {
            listener hook;
            pw_metadata_events events;
            std::map<const std::string, const metadata_property> properties;
        };

        auto proxy = proxy::bind(reinterpret_cast<pw_proxy *>(raw_metadata));
        auto m_state = std::make_shared<state>();

        m_state->events.version = PW_VERSION_METADATA_EVENTS;

        m_state->events.property = [](void *data, std::uint32_t subject, const char *key, const char *type, const char *value) {
            auto &m_state = *reinterpret_cast<state *>(data);
            m_state.properties.emplace(key, metadata_property{type, value, subject});

            return 0;
        };

        // NOLINTNEXTLINE
        pw_metadata_add_listener(raw_metadata, &m_state->hook.get(), &m_state->events, m_state.get());

        return std::async(std::launch::deferred, [m_state, proxy_fut = std::move(proxy)]() mutable -> tl::expected<metadata, error> {
            auto proxy = proxy_fut.get();

            if (!proxy.has_value())
            {
                return tl::make_unexpected(proxy.error());
            }

            return metadata(std::move(proxy.value()), m_state->properties);
        });
    }

    std::map<const std::string, const metadata_property> metadata::properties() const
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