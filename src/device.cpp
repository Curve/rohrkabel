#include "listener.hpp"
#include "device/device.hpp"
#include "registry/registry.hpp"

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct device::impl
    {
        device_info info;
        pw_device *device;
        pw_device_events events;
        std::unique_ptr<listener> hook;
    };

    device::~device()
    {
        if (m_impl)
        {
            pw_proxy_destroy(reinterpret_cast<pw_proxy *>(m_impl->device));
        }
    }

    device::device(device &&device) noexcept : m_impl(std::move(device.m_impl)) {}

    device::device(registry &registry, const global &global) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = PW_VERSION_DEVICE_EVENTS;

        m_impl->events.info = [](void *data, const pw_device_info *info) {
            auto &m_impl = *reinterpret_cast<impl *>(data);
            m_impl.info = {info->props, info->id, info->change_mask, {}};

            for (auto i = 0u; i < info->n_params; i++)
            {
                auto param = info->params[i];
                m_impl.info.params.emplace_back(param_info{param.id, param.user, param.flags});
            }

            m_impl.hook.reset();
        };

        m_impl->hook = std::make_unique<listener>();
        m_impl->device = reinterpret_cast<pw_device *>(pw_registry_bind(registry.get(), global.id, type.c_str(), version, sizeof(void *)));

        // NOLINTNEXTLINE
        pw_device_add_listener(m_impl->device, &m_impl->hook->get(), &m_impl->events, m_impl.get());
    }

    device &device::operator=(device &&node) noexcept
    {
        m_impl = std::move(node.m_impl);
        return *this;
    }

    device_info device::info() const
    {
        return m_impl->info;
    }

    pw_device *device::get() const
    {
        return m_impl->device;
    }

    const std::string device::type = PW_TYPE_INTERFACE_Device;
    const std::uint32_t device::version = PW_VERSION_DEVICE;
} // namespace pipewire