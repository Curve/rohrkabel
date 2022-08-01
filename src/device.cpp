#include "listener.hpp"
#include "device/device.hpp"

#include <optional>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct device::impl
    {
        pw_device *device;
        device_info info;
    };

    device::~device() = default;

    device::device(device &&device) noexcept : proxy(std::move(device)), m_impl(std::move(device.m_impl)) {}

    device::device(proxy &&_proxy, device_info info) : proxy(std::move(_proxy)), m_impl(std::make_unique<impl>())
    {
        m_impl->info = std::move(info);
        m_impl->device = reinterpret_cast<pw_device *>(proxy::get());
    }

    device &device::operator=(device &&device) noexcept
    {
        proxy::operator=(std::move(device));
        m_impl = std::move(device.m_impl);
        return *this;
    }

    lazy_expected<device> device::bind(pw_device *raw_device)
    {
        struct state
        {
            pw_device_events events;
            std::optional<listener> hook;
            std::promise<device_info> info;
        };

        auto proxy = proxy::bind(reinterpret_cast<pw_proxy *>(raw_device));
        auto m_state = std::make_shared<state>();

        m_state->hook.emplace();
        m_state->events.version = PW_VERSION_DEVICE_EVENTS;

        m_state->events.info = [](void *data, const pw_device_info *info) {
            auto &m_state = *reinterpret_cast<state *>(data);
            device_info m_info = {info->props,
                                  info->id,          //
                                  info->change_mask, //
                                  std::vector<param_info>(info->n_params)};

            if (info->params)
            {
                for (auto i = 0u; i < info->n_params; i++)
                {
                    auto param = info->params[i];
                    m_info.params.emplace_back(param_info{param.id, static_cast<param_info_flags>(param.flags)});
                }
            }

            m_state.info.set_value(m_info);
            m_state.hook.reset();
        };

        // NOLINTNEXTLINE
        pw_device_add_listener(raw_device, &m_state->hook->get(), &m_state->events, m_state.get());

        return std::async(std::launch::deferred, [m_state, proxy_fut = std::move(proxy)]() mutable -> tl::expected<device, error> {
            auto proxy = proxy_fut.get();

            if (!proxy.has_value())
            {
                return tl::make_unexpected(proxy.error());
            }

            return device(std::move(proxy.value()), m_state->info.get_future().get());
        });
    }

    void device::set_param(std::uint32_t id, const spa::pod &pod)
    {
        // NOLINTNEXTLINE
        pw_device_set_param(m_impl->device, id, 0, pod.get());
    }

    device_info device::info() const
    {
        return m_impl->info;
    }

    std::future<std::map<std::uint32_t, spa::pod>> device::params() const
    {
        struct state
        {
            listener hook;
            pw_device_events events;
            std::map<std::uint32_t, spa::pod> params;
        };

        auto m_state = std::make_shared<state>();
        m_state->events.version = PW_VERSION_DEVICE_EVENTS;

        for (const auto &param : m_impl->info.params)
        {
            // NOLINTNEXTLINE
            pw_device_enum_params(m_impl->device, 0, param.id, 0, 1, nullptr);
        }

        m_state->events.param = [](void *data, int, uint32_t id, uint32_t, uint32_t, const struct spa_pod *param) {
            auto &m_state = *reinterpret_cast<state *>(data);
            m_state.params.emplace(id, param);
        };

        // NOLINTNEXTLINE
        pw_device_add_listener(m_impl->device, &m_state->hook.get(), &m_state->events, m_state.get());
        return std::async(std::launch::deferred, [m_state] { return std::map<std::uint32_t, spa::pod>(std::move(m_state->params)); });
    }

    pw_device *device::get() const
    {
        return m_impl->device;
    }

    const std::string device::type = PW_TYPE_INTERFACE_Device;
    const std::uint32_t device::version = PW_VERSION_DEVICE;
} // namespace pipewire