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

    template <> lazy_expected<node> core::create(const std::string &factory_name, const properties &props, const std::string &type, std::uint32_t version, update_strategy strategy)
    {
        auto rtn = node::bind(reinterpret_cast<pw_node *>(pw_core_create_object(get(), factory_name.c_str(), type.c_str(), version, &props.get()->dict, sizeof(void *))));
        update(strategy);
        return rtn;
    }

    template <> lazy_expected<link> core::create(const std::string &factory_name, const properties &props, const std::string &type, std::uint32_t version, update_strategy strategy)
    {
        auto rtn = link::bind(reinterpret_cast<pw_link *>(pw_core_create_object(get(), factory_name.c_str(), type.c_str(), version, &props.get()->dict, sizeof(void *))));
        update(strategy);
        return rtn;
    }

    template <>
    lazy_expected<proxy> core::create(const std::string &factory_name, const properties &props, const std::string &type, std::uint32_t version, update_strategy strategy)
    {
        auto rtn = proxy::bind(reinterpret_cast<pw_proxy *>(pw_core_create_object(get(), factory_name.c_str(), type.c_str(), version, &props.get()->dict, sizeof(void *))));
        update(strategy);
        return rtn;
    }

    template <> lazy_expected<link> core::create_simple<link>(std::uint32_t input_port, std::uint32_t output_port, update_strategy strategy)
    {
        auto props = properties{{"link.input.port", std::to_string(input_port)}, {"link.output.port", std::to_string(output_port)}};
        auto rtn = link::bind(reinterpret_cast<pw_link *>(pw_core_create_object(get(), "link-factory", link::type.c_str(), link::version, &props.get()->dict, sizeof(void *))));
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