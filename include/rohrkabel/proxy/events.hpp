#pragma once

#include "proxy.hpp"

#include "../spa/dict.hpp"
#include "../utils/listener.hpp"

#include <memory>

namespace pipewire
{
    enum class proxy_event : std::uint8_t
    {
        error,
        bound,
        bound_props,
    };

    class proxy_listener : public listener<proxy_event,                                                              //
                                           ereignis::event<proxy_event::error, void(int, int, const char *)>,        //
                                           ereignis::event<proxy_event::bound, void(std::uint32_t)>,                 //
                                           ereignis::event<proxy_event::bound_props, void(std::uint32_t, spa::dict)> //
                                           >
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~proxy_listener();

      public:
        proxy_listener(proxy::raw_type *);
        proxy_listener(proxy_listener &&) noexcept;
    };
} // namespace pipewire
