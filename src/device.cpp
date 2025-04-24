#include "device/device.hpp"
#include "device/events.hpp"

#include <pipewire/pipewire.h>
#include <coco/promise/promise.hpp>

namespace pipewire
{
    struct device::impl
    {
        raw_type *device;
        device_info info;
    };

    device::device(proxy &&base, device_info info) : proxy(std::move(base)), m_impl(std::make_unique<impl>())
    {
        m_impl->device = reinterpret_cast<raw_type *>(proxy::get());
        m_impl->info   = std::move(info);
    }

    device::device(device &&) noexcept = default;

    device &device::operator=(device &&) noexcept = default;

    device::~device() = default;

    void device::set_param(std::uint32_t id, std::uint32_t flags, const spa::pod &pod)
    {
        pw_device_set_param(m_impl->device, id, flags, pod.get());
    }

    lazy<device::params_t> device::params() const
    {
        auto listener = listen();
        auto params   = params_t{};

        listener.on<device_event::param>(
            [&](int, uint32_t id, uint32_t, uint32_t, spa::pod param)
            {
                params.emplace(id, std::move(param));
            });

        for (const auto &param : m_impl->info.params)
        {
            pw_device_enum_params(m_impl->device, 0, param.id, 0, 1, nullptr);
        }

        co_await lazy<params_t>::wake_on_await{};
        co_return params;
    }

    device::raw_type *device::get() const
    {
        return m_impl->device;
    }

    device_info device::info() const
    {
        return m_impl->info;
    }

    device::operator raw_type *() const &
    {
        return get();
    }

    task<device> device::bind(raw_type *raw)
    {
        auto _proxy   = proxy::bind(reinterpret_cast<proxy::raw_type *>(raw));
        auto listener = device_listener{raw};

        auto promise = coco::promise<device_info>{};
        auto fut     = promise.get_future();

        listener.once<device_event::info>(
            [promise = std::move(promise)](device_info info) mutable
            {
                promise.set_value(std::move(info));
            });

        auto info  = co_await std::move(fut);
        auto proxy = co_await std::move(_proxy);

        if (!proxy.has_value())
        {
            co_return std::unexpected{proxy.error()};
        }

        co_return device{std::move(proxy.value()), std::move(info)};
    }

    const char *device::type            = PW_TYPE_INTERFACE_Device;
    const std::uint32_t device::version = PW_VERSION_DEVICE;
} // namespace pipewire
