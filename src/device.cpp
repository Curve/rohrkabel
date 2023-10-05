#include "listener.hpp"
#include "device/device.hpp"

#include <optional>
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
            listener hook;
            underlying params;
            pw_device_events events;
        };

        auto m_state            = std::make_shared<state>();
        m_state->events.version = PW_VERSION_DEVICE_EVENTS;

        for (const auto &param : m_impl->info.params)
        {
            pw_device_enum_params(m_impl->device, 0, param.id, 0, 1, nullptr);
        }

        m_state->events.param = [](void *data, int, uint32_t id, uint32_t, uint32_t, const struct spa_pod *param)
        {
            auto &m_state = *reinterpret_cast<state *>(data);
            m_state.params.emplace(id, spa::pod::copy(param));
        };

        pw_device_add_listener(m_impl->device, m_state->hook.get(), &m_state->events, m_state.get());

        return make_lazy<underlying>([m_state]() { return std::move(m_state->params); });
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
            pw_device_events events;
            std::optional<listener> hook;
            std::promise<device_info> info;
        };

        auto proxy   = proxy::bind(reinterpret_cast<pw_proxy *>(raw));
        auto m_state = std::make_shared<state>();

        m_state->hook.emplace();
        m_state->events.version = PW_VERSION_DEVICE_EVENTS;

        m_state->events.info = [](void *data, const pw_device_info *info)
        {
            auto &m_state = *reinterpret_cast<state *>(data);

            device_info m_info = {
                info->props,
                info->id,
                info->change_mask,
                std::vector<param_info>(info->n_params),
            };

            for (auto i = 0u; info->params && i < info->n_params; i++)
            {
                auto param = info->params[i];
                m_info.params.emplace_back(param_info{param.id, static_cast<param_flags>(param.flags)});
            }

            m_state.info.set_value(m_info);
            m_state.hook.reset();
        };

        // NOLINTNEXTLINE(*-optional-access)
        pw_device_add_listener(raw, m_state->hook->get(), &m_state->events, m_state.get());

        return make_lazy<expected<device>>(
            [m_state, fut = std::move(proxy)]() mutable -> expected<device>
            {
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