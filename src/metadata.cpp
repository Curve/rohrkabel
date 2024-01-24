#include "listener.hpp"
#include "metadata/metadata.hpp"

#include <pipewire/pipewire.h>
#include <pipewire/extensions/metadata.h>

namespace pipewire
{
    struct metadata::impl
    {
        pw_metadata *metadata;
        properties_t properties;
    };

    metadata::~metadata() = default;

    metadata::metadata(metadata &&other) noexcept : proxy(std::move(other)), m_impl(std::move(other.m_impl)) {}

    metadata::metadata(proxy &&base, properties_t properties) : proxy(std::move(base)), m_impl(std::make_unique<impl>())
    {
        m_impl->metadata   = reinterpret_cast<pw_metadata *>(proxy::get());
        m_impl->properties = std::move(properties);
    }

    metadata &metadata::operator=(metadata &&other) noexcept
    {
        proxy::operator=(std::move(other));
        m_impl = std::move(other.m_impl);
        return *this;
    }

    void metadata::set_property(std::uint32_t id, std::string key, std::string type, std::string value)
    {
        pw_metadata_set_property(m_impl->metadata, id, key.c_str(), type.c_str(), value.c_str());
        m_impl->properties.emplace(std::move(key), metadata_property{std::move(type), std::move(value), id});
    }

    pw_metadata *metadata::get() const
    {
        return m_impl->metadata;
    }

    metadata::properties_t metadata::properties() const
    {
        return m_impl->properties;
    }

    metadata::operator pw_metadata *() const &
    {
        return get();
    }

    lazy<expected<metadata>> metadata::bind(pw_metadata *raw)
    {
        struct state
        {
            listener hook;
            properties_t properties;
            pw_metadata_events events;
        };

        auto proxy   = proxy::bind(reinterpret_cast<pw_proxy *>(raw));
        auto m_state = std::make_shared<state>();

        m_state->events.version = PW_VERSION_METADATA_EVENTS;

        m_state->events.property =
            [](void *data, std::uint32_t subject, const char *key, const char *type, const char *value)
        {
            auto &m_state = *reinterpret_cast<state *>(data);
            m_state.properties.emplace(key, metadata_property{type, value, subject});

            return 0;
        };

        pw_metadata_add_listener(raw, m_state->hook.get(), &m_state->events, m_state.get());

        return make_lazy<expected<metadata>>(
            [m_state, fut = std::move(proxy)]() mutable -> expected<metadata>
            {
                auto proxy = fut.get();

                if (!proxy.has_value())
                {
                    return tl::make_unexpected(proxy.error());
                }

                return metadata{std::move(proxy.value()), m_state->properties};
            });
    }

    const char *metadata::type            = PW_TYPE_INTERFACE_Metadata;
    const std::uint32_t metadata::version = PW_VERSION_METADATA;
} // namespace pipewire
