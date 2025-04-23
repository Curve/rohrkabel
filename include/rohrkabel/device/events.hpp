#pragma once

#include "info.hpp"
#include "device.hpp"

#include "../spa/pod/pod.hpp"
#include "../utils/listener.hpp"

#include <memory>

namespace pipewire
{
    enum class device_event : std::uint8_t
    {
        info,
        param,
    };

    class device_listener
        : public listener<
              ereignis::event<device_event::info, void(const device_info &)>,                                                //
              ereignis::event<device_event::param, void(int, std::uint32_t, std::uint32_t, std::uint32_t, const spa::pod &)> //
              >
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        device_listener(device::raw_type *);

      public:
        ~device_listener();

      public:
        static const std::uint32_t version;
    };
} // namespace pipewire
