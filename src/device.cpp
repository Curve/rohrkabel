#include "listener.hpp"
#include "device/device.hpp"
#include "registry/registry.hpp"

#include <optional>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct device::impl
    {
        device_info info;
        pw_device *device;
        pw_device_events events;
        std::optional<listener> hook;
    };

    device::~device() = default;

    device::device(pw_device *device) : proxy(reinterpret_cast<pw_proxy *>(device)), m_impl(std::make_unique<impl>())
    {
        m_impl->device = device;
        m_impl->events.version = PW_VERSION_DEVICE_EVENTS;

        m_impl->events.info = [](void *data, const pw_device_info *info) {
            auto &m_impl = *reinterpret_cast<impl *>(data);
            m_impl.info = {info->props, info->id, info->change_mask, std::vector<param_info>(info->n_params)};

            if (info->params)
            {
                for (auto i = 0u; i < info->n_params; i++)
                {
                    auto param = info->params[i];
                    m_impl.info.params.emplace_back(param_info{param.id, static_cast<param_info_flags>(param.flags)});
                }
            }

            m_impl.hook.reset();
        };

        m_impl->hook.emplace();

        // NOLINTNEXTLINE
        pw_device_add_listener(m_impl->device, &m_impl->hook->get(), &m_impl->events, m_impl.get());
    }

    device::device(device &&device) noexcept : proxy(std::move(device)), m_impl(std::move(device.m_impl)) {}

    device::device(registry &registry, std::uint32_t id) : device(reinterpret_cast<pw_device *>(pw_registry_bind(registry.get(), id, type.c_str(), version, sizeof(void *)))) {}

    device &device::operator=(device &&node) noexcept
    {
        proxy::operator=(std::move(node));
        m_impl = std::move(node.m_impl);
        return *this;
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

    device::params_t device::params() const
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