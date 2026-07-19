#include "module/impl/events.hpp"

#include <pipewire/pipewire.h>
#include <pipewire/impl-module.h>

namespace pipewire::impl
{
    struct module_listener::impl
    {
        pw_impl_module_events events;
    };

    module_listener::module_listener(module::raw_type *module) : m_impl(std::make_unique<impl>())
    {
        m_impl->events.version = version;

        m_impl->events.destroy = [](void *data)
        {
            auto &events = *reinterpret_cast<listener::events *>(data);
            events.get<module_event::destroy>().fire();
        };

        m_impl->events.free = [](void *data)
        {
            auto &events = *reinterpret_cast<listener::events *>(data);
            events.get<module_event::free>().fire();
        };

        m_impl->events.initialized = [](void *data)
        {
            auto &events = *reinterpret_cast<listener::events *>(data);
            events.get<module_event::initialized>().fire();
        };

        m_impl->events.registered = [](void *data)
        {
            auto &events = *reinterpret_cast<listener::events *>(data);
            events.get<module_event::registered>().fire();
        };

        pw_impl_module_add_listener(module, listener::get(), &m_impl->events, m_events.get());
    }

    module_listener::~module_listener() = default;

    const std::uint32_t module_listener::version = PW_VERSION_IMPL_MODULE_EVENTS;
} // namespace pipewire::impl
