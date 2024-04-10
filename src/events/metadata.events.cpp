#include "metadata/events.hpp"
#include "metadata/metadata.hpp"

#include <pipewire/pipewire.h>
#include <pipewire/extensions/metadata.h>

namespace pipewire
{
    struct metadata_listener::impl
    {
        pw_metadata_events events;
    };

    metadata_listener::~metadata_listener() = default;

    metadata_listener::metadata_listener(pw_metadata *metadata) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = PW_VERSION_METADATA_EVENTS;

        m_impl->events.property = [](void *data, std::uint32_t subject, const char *key, const char *type,
                                     const char *value) {
            auto &events  = *reinterpret_cast<listener::events *>(data);
            auto property = metadata_property{type, value, subject};

            return events.at<metadata_event::property>().during(0, key, std::move(property)).value_or(0);
        };

        pw_metadata_add_listener(metadata, listener::get(), &m_impl->events, m_events.get());
    }

    metadata_listener::metadata_listener(metadata_listener &&metadata_listener) noexcept
        : listener(std::move(metadata_listener)), m_impl(std::move(metadata_listener.m_impl))
    {
    }
} // namespace pipewire
