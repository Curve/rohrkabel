#include "core/core.hpp"
#include "loop/main.hpp"

#include <cassert>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct core::impl
    {
        pw_core *core;
    };

    template <> void core::update<update_strategy::sync>()
    {
        bool done = false;
        int pending = sync(0);
        auto listener = listen<core_listener>();

        listener.on<core_event::done>([&](std::uint32_t id, int seq) {
            if (id == PW_ID_CORE && seq == pending)
            {
                done = true;
                m_context.get_loop().quit();
            }
        });

        while (!done)
        {
            m_context.get_loop().run();
        }
    }

    template <> void core::update<update_strategy::none>() {}

    core::~core()
    {
        pw_core_disconnect(m_impl->core);
    }

    core::core(context &context) : m_context(context), m_impl(std::make_unique<impl>())
    {
        m_impl->core = pw_context_connect(context.get(), nullptr, 0);
        assert((void("Failed to connect core"), m_impl->core));
    }

    void core::update(update_strategy strategy)
    {
        switch (strategy)
        {
        case update_strategy::none:
            update<update_strategy::none>();
            break;
        case update_strategy::sync:
            update<update_strategy::sync>();
            break;
        }
    }

    int core::sync(int seq)
    {
        // NOLINTNEXTLINE
        return pw_core_sync(m_impl->core, PW_ID_CORE, seq);
    }

    template <> core_listener core::listen()
    {
        return {*this};
    }

    template <> node core::create(const std::string &factory_name, const properties &props, const std::string &type, std::uint32_t version, update_strategy strategy)
    {
        node rtn{reinterpret_cast<pw_node *>(pw_core_create_object(get(), factory_name.c_str(), type.c_str(), version, &props.get()->dict, sizeof(void *)))};
        update(strategy);
        return rtn;
    }

    template <> proxy core::create(const std::string &factory_name, const properties &props, const std::string &type, std::uint32_t version, update_strategy strategy)
    {
        proxy rtn{reinterpret_cast<pw_proxy *>(pw_core_create_object(get(), factory_name.c_str(), type.c_str(), version, &props.get()->dict, sizeof(void *)))};
        update(strategy);
        return rtn;
    }

    template <> link_factory core::create_simple<link_factory>(std::uint32_t input, std::uint32_t output, update_strategy strategy)
    {
        link_factory rtn{*this, input, output};
        update(strategy);
        return rtn;
    }

    pw_core *core::get() const
    {
        return m_impl->core;
    }

    context &core::get_context()
    {
        return m_context;
    }
} // namespace pipewire