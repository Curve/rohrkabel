#include "metadata/events.hpp"

#include <pipewire/pipewire.h>
#include <pipewire/extensions/metadata.h>

namespace pipewire
{
    struct metadata_listener::impl
    {
        pw_metadata_events events;
    };

    metadata_listener::metadata_listener(metadata::raw_type *metadata) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = version;

        m_impl->events.property = [](void *data, std::uint32_t subject, const char *key, const char *type, const char *value)
        {
            auto &events  = *reinterpret_cast<listener::events *>(data);
            auto property = metadata_property{
                .type    = type ? type : "",
                .value   = value ? value : "",
                .subject = subject,
            };

            return events.get<metadata_event::property>().fire(key, std::move(property)).skip(0).value_or(0);
        };

        pw_metadata_add_listener(metadata, listener::get(), &m_impl->events, m_events.get());
    }

    metadata_listener::~metadata_listener() = default;

    const std::uint32_t metadata_listener::version = PW_VERSION_METADATA_EVENTS;
} // namespace pipewire
