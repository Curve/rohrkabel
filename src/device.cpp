#include "device/device.hpp"
#include "device/events.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct device::impl
    {
        device_info info;
        pw_device *device;
    };

    device::~device() = default;

    device::device(device &&other) noexcept : proxy(std::move(other)), m_impl(std::move(other.m_impl)) {}

    device::device(proxy &&base, device_info info) : proxy(std::move(base)), m_impl(std::make_unique<impl>())
    {
        m_impl->device = reinterpret_cast<pw_device *>(proxy::get());
        m_impl->info   = std::move(info);
    }

    device &device::operator=(device &&other) noexcept
    {
        proxy::operator=(std::move(other));
        m_impl = std::move(other.m_impl);
        return *this;
    }

    void device::set_param(std::uint32_t id, std::uint32_t flags, const spa::pod &pod)
    {
        pw_device_set_param(m_impl->device, id, flags, pod.get());
    }

    lazy<device::underlying> device::params() const
    {
        struct state
        {
            device_listener listener;

          public:
            underlying params;
        };

        auto m_state    = std::make_shared<state>(get());
        auto weak_state = std::weak_ptr{m_state};

        m_state->listener.on<device_event::param>([weak_state](int, uint32_t id, uint32_t, uint32_t, spa::pod param) {
            weak_state.lock()->params.emplace(id, std::move(param));
        });

        for (const auto &param : m_impl->info.params)
        {
            pw_device_enum_params(m_impl->device, 0, param.id, 0, 1, nullptr);
        }

        return make_lazy<underlying>([m_state]() -> underlying {
            return m_state->params;
        });
    }

    pw_device *device::get() const
    {
        return m_impl->device;
    }

    device_info device::info() const
    {
        return m_impl->info;
    }

    device::operator pw_device *() const &
    {
        return get();
    }

    lazy<expected<device>> device::bind(pw_device *raw)
    {
        struct state
        {
            device_listener listener;

          public:
            std::promise<device_info> info;
        };

        auto proxy = proxy::bind(reinterpret_cast<pw_proxy *>(raw));

        auto m_state    = std::make_shared<state>(raw);
        auto weak_state = std::weak_ptr{m_state};

        m_state->listener.once<device_event::info>([weak_state](device_info info) {
            weak_state.lock()->info.set_value(std::move(info));
        });

        return make_lazy<expected<device>>([m_state, fut = std::move(proxy)]() mutable -> expected<device> {
            auto proxy = fut.get();

            if (!proxy.has_value())
            {
                return tl::make_unexpected(proxy.error());
            }

            return device{std::move(proxy.value()), m_state->info.get_future().get()};
        });
    }

    const char *device::type            = PW_TYPE_INTERFACE_Device;
    const std::uint32_t device::version = PW_VERSION_DEVICE;
} // namespace pipewire
