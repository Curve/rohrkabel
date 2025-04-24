#pragma once

#include "metadata.hpp"
#include "../utils/listener.hpp"

#include <memory>

namespace pipewire
{
    enum class metadata_event : std::uint8_t
    {
        property,
    };

    class metadata_listener : public listener<                                                                    //
                                  ereignis::event<metadata_event::property, int(const char *, metadata_property)> //
                                  >
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        metadata_listener(metadata::raw_type *);

      public:
        ~metadata_listener();

      public:
        static const std::uint32_t version;
    };
} // namespace pipewire
