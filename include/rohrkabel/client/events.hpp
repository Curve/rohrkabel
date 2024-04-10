#pragma once

#include "info.hpp"
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
        : public listener<
              client_event,                                                                                        //
              ereignis::event<client_event::info, void(client_info)>,                                              //
              ereignis::event<client_event::permission, void(std::uint32_t, std::uint32_t, const pw_permission *)> //
              >
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~client_listener();

      public:
        client_listener(pw_client *);
        client_listener(client_listener &&) noexcept;
    };
} // namespace pipewire
