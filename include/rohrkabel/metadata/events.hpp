#pragma once

#include "metadata.hpp"

#include "property.hpp"
#include "../listener.hpp"

#include <memory>
#include <ereignis/manager.hpp>

namespace pipewire
{
    enum class metadata_event : std::uint8_t
    {
        property,
    };

    class metadata_listener : listener
    {
        struct impl;

      private:
        using events = ereignis::manager<                                                   //
            ereignis::event<metadata_event::property, int(const char *, metadata_property)> //
            >;

      private:
        events m_events;
        std::unique_ptr<impl> m_impl;

      public:
        ~metadata_listener();

      public:
        metadata_listener(pw_metadata *);
        metadata_listener(metadata_listener &&) noexcept;

      public:
        void clear(metadata_event event);
        void remove(metadata_event event, std::uint64_t id);

      public:
        template <metadata_event Event>
        std::uint64_t on(events::type_t<Event> &&) = delete;
    };

    template <>
    std::uint64_t metadata_listener::on<metadata_event::property>(events::type_t<metadata_event::property> &&);
} // namespace pipewire
