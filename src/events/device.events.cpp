#include "device/device.hpp"
#include "device/events.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct device_listener::impl
    {
        pw_device_events events;
    };

    device_listener::~device_listener() = default;

    device_listener::device_listener(device::raw_type *device) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = PW_VERSION_DEVICE_EVENTS;

        m_impl->events.info = [](void *data, const pw_device_info *info) {
            auto &thiz = *reinterpret_cast<device_listener *>(data);
            thiz.m_events.at<device_event::info>().fire(device_info::from(info));
        };

        m_impl->events.param = [](void *data, int seq, uint32_t id, uint32_t index, uint32_t next,
                                  const struct spa_pod *param) {
            auto &thiz = *reinterpret_cast<device_listener *>(data);
            thiz.m_events.at<device_event::param>().fire(seq, id, index, next, spa::pod::copy(param));
        };

        pw_device_add_listener(device, listener::get(), &m_impl->events, this);
    }

    device_listener::device_listener(device_listener &&device_listener) noexcept
        : listener(std::move(device_listener)), m_impl(std::move(device_listener.m_impl))
    {
    }
} // namespace pipewire
