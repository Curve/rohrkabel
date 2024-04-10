#pragma once

#include "info.hpp"
#include "link.hpp"

#include "../utils/listener.hpp"

#include <memory>

namespace pipewire
{
    enum class link_event : std::uint8_t
    {
        info,
    };

    class link_listener : public listener<link_event,                                        //
                                          ereignis::event<link_event::info, void(link_info)> //
                                          >
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~link_listener();

      public:
        link_listener(pw_link *);
        link_listener(link_listener &&) noexcept;
    };
} // namespace pipewire
