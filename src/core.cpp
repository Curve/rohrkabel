#include "core/core.hpp"
#include "core/events.hpp"

#include "link/link.hpp"
#include "node/node.hpp"

#include <ranges>
#include <limits>
#include <format>

#include <pipewire/pipewire.h>
#include <coco/promise/promise.hpp>

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

    void *core::create_object(object factory) const
    {
        const auto &[name, props, type, version] = factory;
        const auto *dict                         = &props.get()->dict;
        return pw_core_create_object(get(), name.c_str(), type->c_str(), version.value(), dict, 0);
    }

    int core::sync(int seq) const
    {
        auto expected = m_impl->last_seq.load();
        auto updated  = pw_core_sync(m_impl->core.get(), core_id, seq);

        while (expected < updated && !m_impl->last_seq.compare_exchange_weak(expected, updated))
        {
            // Assumes that pw_core_sync only returns increasing ids (!)
        }

        return updated;
    }

    template lazy<int> core::sync<sync_mode::basic>() const;
    template lazy<int> core::sync<sync_mode::recursive>() const;

    template <sync_mode T>
    lazy<int> core::sync() const
    {
        auto listener                 = listen();
        [[maybe_unused]] auto pending = std::numeric_limits<int>::max();

        auto promise = coco::promise<int>{};
        auto fut     = promise.get_future();
        auto done    = std::atomic<bool>{false};

        listener.on<core_event::done>(
            [&](auto id, auto seq) mutable
            {
                if (id != core_id || seq < (T == sync_mode::basic ? pending : m_impl->last_seq.load()))
                {
                    return;
                }

                if (done.exchange(true))
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
        auto loop = context()->loop();

        coco::then(sync<sync_mode::recursive>(),
                   [&](auto)
                   {
                       loop->quit();
                   });

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
    task<node> core::create(null_factory factory)
    {
        using enum null_factory::kind;

        const auto positions = factory.positions            //
                               | std::views::join_with(',') //
                               | std::ranges::to<std::string>();

        auto props = std::move(factory.props);

        props.set("factory.name", "support.null-audio-sink");
        props.set("node.name", factory.name);
        props.set("media.class", factory.type == sink ? "Audio/Sink/Virtual" : "Audio/Source/Virtual");
        props.set("audio.position", positions);
        props.set("audio.channels", std::format("{}", factory.positions.size()));

        return create<node>({.name = "adapter", .props = std::move(props)});
    }

    std::shared_ptr<pipewire::context> core::context() const
    {
        return m_impl->context;
    }

    core::raw_type *core::get() const
    {
        return m_impl->core.get();
    }

    core::raw_type *core::release() &&
    {
        return m_impl->core.release();
    }

    core::operator raw_type *() const &
    {
        return get();
    }

    res<std::shared_ptr<core>, std::error_code> core::create(std::shared_ptr<pipewire::context> context)
    {
        auto *const core = pw_context_connect(context->get(), nullptr, 0);

        if (!core)
        {
            return std::unexpected{std::make_error_code(static_cast<std::errc>(errno))};
        }

        return from(core, std::move(context));
    }

    std::shared_ptr<core> core::from(raw_type *core, std::shared_ptr<pipewire::context> context)
    {
        static constexpr auto deleter = [](auto *core)
        {
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
