#pragma once

#include "link.hpp"
#include "../utils/listener.hpp"

#include <memory>

namespace pipewire
{
    enum class link_event : std::uint8_t
    {
        info,
    };

    class link_listener : public listener<                                       //
                              ereignis::event<link_event::info, void(link_info)> //
                              >
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        link_listener(link::raw_type *);

      public:
        ~link_listener();

      public:
        static const std::uint32_t version;
    };
} // namespace pipewire
