#include "metadata/events.hpp"
#include "metadata/metadata.hpp"

#include <pipewire/pipewire.h>
#include <pipewire/extensions/metadata.h>

namespace pipewire
{
    struct metadata::impl
    {
        raw_type *metadata;
        properties_t properties;
    };

    metadata::~metadata() = default;

    metadata::metadata(metadata &&other) noexcept : proxy(std::move(other)), m_impl(std::move(other.m_impl)) {}

    metadata::metadata(proxy &&base, properties_t properties) : proxy(std::move(base)), m_impl(std::make_unique<impl>())
    {
        m_impl->metadata   = reinterpret_cast<raw_type *>(proxy::get());
        m_impl->properties = std::move(properties);
    }

    metadata &metadata::operator=(metadata &&other) noexcept
    {
        proxy::operator=(std::move(other));
        m_impl = std::move(other.m_impl);
        return *this;
    }

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

    lazy<expected<metadata>> metadata::bind(raw_type *raw)
    {
        struct state
        {
            metadata_listener listener;

          public:
            properties_t properties;
        };

        auto proxy = proxy::bind(reinterpret_cast<proxy::raw_type *>(raw));

        auto m_state    = std::make_shared<state>(raw);
        auto weak_state = std::weak_ptr{m_state};

        m_state->listener.on<metadata_event::property>([weak_state](auto key, auto property) {
            weak_state.lock()->properties.emplace(key, std::move(property));
            return 0;
        });

        return make_lazy<expected<metadata>>([m_state, fut = std::move(proxy)]() mutable -> expected<metadata> {
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
