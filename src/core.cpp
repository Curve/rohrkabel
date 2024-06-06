#include "core/core.hpp"
#include "core/events.hpp"

#include "utils/check.hpp"

#include "link/link.hpp"
#include "node/node.hpp"
#include "proxy/proxy.hpp"

#include <format>
#include <optional>

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct core::impl
    {
        pw_unique_ptr<raw_type> core;
        std::shared_ptr<pipewire::context> context;
    };

    core::~core() = default;

    core::core(deleter<raw_type> deleter, raw_type *raw, std::shared_ptr<pipewire::context> context)
        : m_impl(std::make_unique<impl>(pw_unique_ptr<raw_type>{raw, deleter}, std::move(context)))
    {
    }

    void *core::create(factory factory) const
    {
        const auto &[name, props, type, version] = factory;
        const auto *dict                         = &props.get()->dict;

        // NOLINTNEXTLINE(*-optional-access)
        return pw_core_create_object(get(), name.c_str(), type->c_str(), version.value(), dict, 0);
    }

    template <>
    cancellable_lazy<expected<bool>> core::update<update_strategy::none>()
    {
        return make_cancellable_lazy<expected<bool>>([](auto...) -> expected<bool> {
            return true;
        });
    }

    template <>
    cancellable_lazy<expected<bool>> core::update<update_strategy::sync>()
    {
        struct state
        {
            core_listener listener;

          public:
            int pending;

          public:
            std::optional<std::variant<bool, error>> result;
        };

        auto m_state    = std::make_shared<state>(m_impl->core.get());
        auto weak_state = std::weak_ptr{m_state};

        auto loop = m_impl->context->loop();

        m_state->listener.on<core_event::done>([loop, weak_state](auto id, auto seq) {
            if (id != core_id || seq != weak_state.lock()->pending)
            {
                return;
            }

            weak_state.lock()->result.emplace(true);
            loop->quit();
        });

        m_state->listener.on<core_event::error>([loop, weak_state](auto id, const auto &error) {
            if (id != core_id)
            {
                return;
            }

            check(false, error.message);

            weak_state.lock()->result.emplace(error);
            loop->quit();
        });

        m_state->pending = sync(0);

        return make_cancellable_lazy<expected<bool>>([loop, m_state](auto token) -> expected<bool> {
            while (!token.stop_requested() && !m_state->result.has_value())
            {
                loop->run();
            }

            auto result = m_state->result.value_or(false);

            if (std::holds_alternative<error>(result))
            {
                return tl::make_unexpected(std::get<error>(result));
            }

            return std::get<bool>(result);
        });
    }

    cancellable_lazy<expected<bool>> core::update(update_strategy strategy)
    {
        if (strategy == update_strategy::sync)
        {
            return update<update_strategy::sync>();
        }

        return update<update_strategy::none>();
    }

    int core::sync(int seq)
    {
        return pw_core_sync(m_impl->core.get(), core_id, seq);
    }

    template <>
    lazy<expected<proxy>> core::create(factory factory, update_strategy strategy)
    {
        if (!factory.version.has_value() || !factory.type.has_value())
        {
            return make_lazy<expected<proxy>>([]() -> expected<proxy> {
                return tl::make_unexpected(error{.message = "Bad Factory"});
            });
        }

        auto *proxy = create(std::move(factory));
        auto rtn    = proxy::bind(reinterpret_cast<proxy::raw_type *>(proxy));

        update(strategy);

        return rtn;
    }

    template <>
    lazy<expected<link>> core::create(link_factory factory, update_strategy strategy)
    {
        auto props = properties::create({
            {"link.input.port", std::to_string(factory.input)},
            {"link.output.port", std::to_string(factory.output)},
        });

        return create<link>({.name = "link-factory", .props = std::move(props)}, strategy);
    }

    template <>
    lazy<expected<node>> core::create(null_sink_factory factory, update_strategy strategy)
    {
        std::string positions;

        for (const auto &position : factory.positions)
        {
            positions += std::format("{},", position);
        }

        positions.pop_back();

        auto props = properties::create({
            {"node.name", factory.name},
            {"media.class", "Audio/Source/Virtual"},
            {"factory.name", "support.null-audio-sink"},
            {"audio.channels", std::to_string(factory.positions.size())},
            {"audio.position", positions},
        });

        return create<node>({.name = "adapter", .props = std::move(props)}, strategy);
    }

    core::raw_type *core::get() const
    {
        return m_impl->core.get();
    }

    std::shared_ptr<pipewire::context> core::context() const
    {
        return m_impl->context;
    }

    core::operator raw_type *() const &
    {
        return get();
    }

    std::shared_ptr<core> core::create(std::shared_ptr<pipewire::context> context)
    {
        auto *core = pw_context_connect(context->get(), nullptr, 0);
        check(core, "Failed to connect core");

        if (!core)
        {
            return nullptr;
        }

        return from(core, std::move(context));
    }

    std::shared_ptr<core> core::from(raw_type *core, std::shared_ptr<pipewire::context> context)
    {
        static constexpr auto deleter = [](auto *core) {
            pw_core_disconnect(core);
        };

        return std::shared_ptr<pipewire::core>(new pipewire::core{deleter, core, std::move(context)});
    }

    std::shared_ptr<core> core::view(raw_type *core, std::shared_ptr<pipewire::context> context)
    {
        return std::shared_ptr<pipewire::core>(new pipewire::core{view_deleter<raw_type>, core, std::move(context)});
    }

    const char *core::type            = PW_TYPE_INTERFACE_Core;
    const std::uint32_t core::version = PW_VERSION_CORE;
    const std::uint32_t core::core_id = PW_ID_CORE;
} // namespace pipewire
