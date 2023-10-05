#include "core/core.hpp"
#include "core/events.hpp"
#include "registry/registry.hpp"

#include "proxy.hpp"
#include "link/link.hpp"

#include <cassert>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct core::impl
    {
        pw_core *core;
        std::shared_ptr<pipewire::context> context;
        std::shared_ptr<pipewire::registry> registry;
    };

    core::~core()
    {
        pw_core_disconnect(m_impl->core);
    }

    core::core(std::shared_ptr<pipewire::context> context) : m_impl(std::make_unique<impl>())
    {
        m_impl->core    = pw_context_connect(context->get(), nullptr, 0);
        m_impl->context = std::move(context);

        assert((void("Failed to connect core"), m_impl->core));
    }

    template <>
    void core::update<update_strategy::none>()
    {
    }

    template <>
    void core::update<update_strategy::sync>()
    {
        auto done    = false;
        auto pending = sync(0);

        auto listener = listen<core_listener>();
        auto loop     = m_impl->context->loop();

        listener.on<core_event::done>(
            [&](auto id, auto seq)
            {
                if (id != PW_ID_CORE || seq != pending)
                {
                    return;
                }

                done = true;
                loop->quit();
            });

        while (!done)
        {
            loop->run();
        }
    }

    void core::update(update_strategy strategy)
    {
        switch (strategy)
        {
        case update_strategy::none:
            return update<update_strategy::none>();
        case update_strategy::sync:
            return update<update_strategy::sync>();
        }
    }

    int core::sync(int seq)
    {
        return pw_core_sync(m_impl->core, PW_ID_CORE, seq);
    }

    template <>
    core_listener core::listen()
    {
        return {*this};
    }

    template <>
    lazy<expected<proxy>> core::create(const factory &factory, update_strategy strategy)
    {
        if (!factory.version.has_value() || !factory.type.has_value())
        {
            return make_lazy<expected<proxy>>([]() -> expected<proxy>
                                              { return tl::make_unexpected(error{.message = "Bad Factory"}); });
        }

        auto version = factory.version.value();
        auto type    = factory.type.value();
        auto *dict   = &factory.props.get()->dict;

        auto *proxy = pw_core_create_object(get(), factory.name.c_str(), type.c_str(), version, dict, 0);
        auto rtn    = proxy::bind(reinterpret_cast<pw_proxy *>(proxy));

        update(strategy);

        return rtn;
    }

    template <>
    lazy<expected<link>> core::create(const link_factory &factory, update_strategy strategy)
    {
        auto props = properties{{"link.input.port", std::to_string(factory.input)},
                                {"link.output.port", std::to_string(factory.output)}};

        return create<link>({.name = "link-factory", .props = std::move(props)}, strategy);
    }

    std::shared_ptr<registry> core::registry()
    {
        if (!m_impl->registry)
        {
            m_impl->registry = std::shared_ptr<pipewire::registry>(new pipewire::registry{shared_from_this()});
        }

        return m_impl->registry;
    }

    pw_core *core::get() const
    {
        return m_impl->core;
    }

    std::shared_ptr<pipewire::context> core::context() const
    {
        return m_impl->context;
    }

    core::operator pw_core *() const &
    {
        return get();
    }
} // namespace pipewire