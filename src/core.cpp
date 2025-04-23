#include "core/core.hpp"
#include "core/events.hpp"

#include "link/link.hpp"
#include "node/node.hpp"

#include "utils/check.hpp"

#include <ranges>

#include <coco/utils/utils.hpp>
#include <coco/promise/promise.hpp>

#include <pipewire/pipewire.h>

namespace pipewire
{
    struct core::impl
    {
        pw_unique_ptr<raw_type> core;
        std::shared_ptr<pipewire::context> context;

      public:
        std::atomic<int> last_seq;
    };

    core::core(deleter<raw_type> deleter, raw_type *raw, std::shared_ptr<pipewire::context> context)
        : m_impl(std::make_unique<impl>(pw_unique_ptr<raw_type>{raw, deleter}, std::move(context)))
    {
    }

    core::~core() = default;

    void *core::create_object(factory factory) const
    {
        const auto &[name, props, type, version] = factory;
        const auto *dict                         = &props.get()->dict;
        return pw_core_create_object(get(), name.c_str(), type->c_str(), version.value(), dict, 0);
    }

    int core::sync(int seq) const
    {
        return pw_core_sync(m_impl->core.get(), core_id, seq);
    }

    lazy<int> core::sync() const
    {
        auto listener = listen();
        auto pending  = 0;

        auto promise = coco::promise<int>{};
        auto fut     = promise.get_future();

        listener.on<core_event::done>([&](auto id, auto seq) {
            if (id != core_id || seq != pending)
            {
                return;
            }

            promise.set_value(seq);
        });

        pending = sync(0);

        co_return co_await std::move(fut);
    }

    void core::run_once() const
    {
        auto listener = listen();
        auto loop     = context()->loop();

        listener.on<core_event::done>([&](auto id, auto seq) {
            if (id != core_id || seq != m_impl->last_seq)
            {
                return;
            }

            loop->quit();
        });

        m_impl->last_seq = sync(0);

        loop->run();
    }

    template <>
    task<link> core::create(link_factory factory)
    {
        auto props = properties::create({
            {"link.input.port", std::to_string(factory.input)},
            {"link.output.port", std::to_string(factory.output)},
        });

        return create<link>({.name = "link-factory", .props = std::move(props)});
    }

    template <>
    task<node> core::create(null_sink_factory factory)
    {
        auto positions = factory.positions            //
                         | std::views::join_with(',') //
                         | std::ranges::to<std::string>();

        auto props = properties::create({
            {"node.name", factory.name},
            {"media.class", "Audio/Source/Virtual"},
            {"factory.name", "support.null-audio-sink"},
            {"audio.channels", std::to_string(factory.positions.size())},
            {"audio.position", positions},
        });

        return create<node>({.name = "adapter", .props = std::move(props)});
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
