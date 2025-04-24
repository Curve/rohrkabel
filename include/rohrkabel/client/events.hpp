#pragma once

#include "client.hpp"
#include "../utils/listener.hpp"

#include <memory>

struct pw_permission;

namespace pipewire
{
    enum class client_event : std::uint8_t
    {
        info,
        permission,
    };

    class client_listener
        : public listener<                                                                                         //
              ereignis::event<client_event::info, void(client_info)>,                                              //
              ereignis::event<client_event::permission, void(std::uint32_t, std::uint32_t, const pw_permission *)> //
              >
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        client_listener(client::raw_type *);

      public:
        ~client_listener();

      public:
        static const std::uint32_t version;
    };
} // namespace pipewire
