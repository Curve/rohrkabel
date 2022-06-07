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

        std::uint32_t last_id;
        std::map<std::uint32_t, spa::pod> params;
    };

    device::~device()
    {
        if (m_impl)
        {
            pw_proxy_destroy(reinterpret_cast<pw_proxy *>(m_impl->device));
        }
    }

    device::device(device &&device) noexcept : m_impl(std::move(device.m_impl)) {}

    device::device(registry &registry, std::uint32_t id) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = PW_VERSION_DEVICE_EVENTS;

        m_impl->events.info = [](void *data, const pw_device_info *info) {
            auto &m_impl = *reinterpret_cast<impl *>(data);
            m_impl.info = {info->props, info->id, info->change_mask};

            if (info->params)
            {
                m_impl.last_id = info->params[info->n_params - 1].id;

                for (auto i = 0u; i < info->n_params; i++)
                {
                    auto param = info->params[i];

                    if (param.flags & SPA_PARAM_INFO_READ)
                    {
                        // NOLINTNEXTLINE
                        pw_device_enum_params(m_impl.device, 0, param.id, 0, -1, nullptr);
                    }
                }
            }
        };
        m_impl->events.param = [](void *data, int, uint32_t id, uint32_t, uint32_t, const struct spa_pod *param) {
            auto &m_impl = *reinterpret_cast<impl *>(data);
            m_impl.params.emplace(id, param);

            if (id == m_impl.last_id)
            {
                m_impl.hook.reset();
            }
        };

        m_impl->hook = std::make_unique<listener>();
        m_impl->device = reinterpret_cast<pw_device *>(pw_registry_bind(registry.get(), id, type.c_str(), version, sizeof(void *)));

        // NOLINTNEXTLINE
        pw_device_add_listener(m_impl->device, &m_impl->hook->get(), &m_impl->events, m_impl.get());
    }

    device &device::operator=(device &&node) noexcept
    {
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

    const std::map<std::uint32_t, spa::pod> &device::params() const
    {
        return m_impl->params;
    }

    pw_device *device::get() const
    {
        return m_impl->device;
    }

    const std::string device::type = PW_TYPE_INTERFACE_Device;
    const std::uint32_t device::version = PW_VERSION_DEVICE;
} // namespace pipewire