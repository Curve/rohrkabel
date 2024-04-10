#pragma once

#include "metadata.hpp"

#include "property.hpp"
#include "../utils/listener.hpp"

#include <memory>
#include <ereignis/manager.hpp>

namespace pipewire
{
    enum class metadata_event : std::uint8_t
    {
        property,
    };

    class metadata_listener
        : public listener<metadata_event,                                                                 //
                          ereignis::event<metadata_event::property, int(const char *, metadata_property)> //
                          >
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~metadata_listener();

      public:
        metadata_listener(pw_metadata *);
        metadata_listener(metadata_listener &&) noexcept;
    };
} // namespace pipewire
