#pragma once

#include "info.hpp"
#include "port.hpp"

#include "../utils/listener.hpp"

#include <memory>

namespace pipewire
{
    enum class port_event : std::uint8_t
    {
        info,
        param,
    };

    class port_listener
        : public listener<
              ereignis::event<port_event::info, void(port_info)>,                                                          //
              ereignis::event<port_event::param, void(int, std::uint32_t, std::uint32_t, std::uint32_t, const spa::pod &)> //
              >
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        port_listener(port::raw_type *);

      public:
        ~port_listener();

      public:
        static const std::uint32_t version;
    };
} // namespace pipewire
