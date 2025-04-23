#include <print>
#include <ranges>
#include <iostream>

#include <coco/stray/stray.hpp>
#include <coco/utils/utils.hpp>

#include <rohrkabel/port/port.hpp>
#include <rohrkabel/link/link.hpp>
#include <rohrkabel/node/node.hpp>

#include <rohrkabel/metadata/metadata.hpp>

#include <rohrkabel/registry/events.hpp>
#include <rohrkabel/registry/registry.hpp>

namespace pw = pipewire;

static std::string extract_name(const std::string &value)
{
    const auto delim = value.find(':');

    if (delim == std::string::npos)
    {
        return {};
    }

    const auto start = delim + 2;
    const auto end   = value.rfind('"');

    return value.substr(start, end - start);
}

int main()
{
    auto loop    = pw::main_loop::create();
    auto context = pw::context::create(loop);
    auto core    = pw::core::create(context);
    auto reg     = pw::registry::create(core);

    auto listener = reg->listen();
    auto globals  = std::vector<pw::global>{};

    const auto on_global = [&](const pw::global &global) {
        globals.emplace_back(global);
    };

    listener.on<pw::registry_event::global>(on_global);
    core->run_once();

    auto microphone = std::string{};
    auto speaker    = std::string{};

    const auto bind_globals = [&]<typename T>(std::type_identity<T>) -> coco::task<std::vector<T>> {
        const auto is = [&reg](auto &&x) {
            return x.type == T::type;
        };
        const auto bind = [&reg](auto &&x) {
            return reg->bind<T>(x.id);
        };
        const auto has_value = [&reg](auto &&x) {
            return x.has_value();
        };
        const auto unpack = [&reg](auto &&x) {
            return std::move(x.value());
        };

        co_return co_await coco::when_all(globals                           //
                                          | std::views::filter(is)          //
                                          | std::views::transform(bind)     //
                                          | std::ranges::to<std::vector>()) //
            | std::views::filter(has_value)                                 //
            | std::views::transform(unpack)                                 //
            | std::ranges::to<std::vector>();
    };

    const auto parse_metadata = [&]() -> coco::task<void> {
        auto metadatas = co_await bind_globals(std::type_identity<pw::metadata>{});

        const auto is_default = [](auto &&x) {
            auto props = x.props();
            return props["metadata.name"] == "default";
        };

        const auto def = std::ranges::find_if(metadatas, is_default);

        if (def == metadatas.end())
        {
            co_return;
        }

        auto props = def->properties();

        speaker    = extract_name(props["default.audio.sink"].value);
        microphone = extract_name(props["default.audio.source"].value);
    };

    const auto run_while = [&](auto awaitable) {
        coco::then(std::move(awaitable), [&] {
            loop->quit();
        });

        loop->run();
    };

    run_while(parse_metadata());

    if (microphone.empty() || speaker.empty())
    {
        std::println("Failed to parse default devices from metadata");
        return 1;
    }

    auto microphone_node = std::optional<pw::node>{};
    auto speaker_node    = std::optional<pw::node>{};

    const auto find_nodes = [&]() -> coco::task<void> {
        auto nodes = co_await bind_globals(std::type_identity<pw::node>{});

        const auto is = [&](auto what) {
            return [what](auto &&x) {
                auto props = x.props();
                return props["node.name"] == what;
            };
        };

        const auto _speaker    = std::ranges::find_if(nodes, is(speaker));
        const auto _microphone = std::ranges::find_if(nodes, is(microphone));

        if (_speaker == nodes.end() || _microphone == nodes.end())
        {
            co_return;
        }

        speaker_node.emplace(std::move(*_speaker));
        microphone_node.emplace(std::move(*_microphone));
    };

    run_while(find_nodes());

    if (!speaker_node || !microphone_node)
    {
        std::println("Could not find speaker or microphone node\n");
    }

    auto virt_mic = std::optional<pw::node>{};

    const auto create_microphone = [&]() -> coco::task<void> {
        auto created = co_await core->create(pw::null_sink_factory{
            .name      = "Virtual Mic",
            .positions = {"FL", "FR"},
        });

        if (!created)
        {
            std::println("Failed to create null sink: {}", created.error().message);
            co_return;
        }

        virt_mic.emplace(std::move(created.value()));
    };

    const auto count_ports = [&]() {
        return std::ranges::count_if(globals, [](auto &&x) {
            return x.type == pw::port::type;
        });
    };

    const auto prev_ports = count_ports();
    run_while(create_microphone());

    if (!virt_mic)
    {
        return 1;
    }

    // The port creation of the null-sink is slightly delayed.
    while (count_ports() == prev_ports)
    {
        core->run_once();
    }

    const auto virt_mic_id = std::format("{}", virt_mic->id());

    auto virt_in_fl = std::optional<pw::port>{};
    auto virt_in_fr = std::optional<pw::port>{};

    auto virt_out_fl = std::optional<pw::port>{};
    auto virt_out_fr = std::optional<pw::port>{};

    const auto find_ports = [&]() -> coco::task<void> {
        for (auto &&port : co_await bind_globals(std::type_identity<pw::port>{}))
        {
            auto props = port.props();

            if (props["node.id"] != virt_mic_id)
            {
                continue;
            }

            const auto channel = props["port.id"];
            const auto info    = port.info();

            if (channel == "0")
            {
                (info.direction == pw::port_direction::input ? virt_in_fl : virt_out_fl).emplace(std::move(port));
            }
            else if (channel == "1")
            {
                (info.direction == pw::port_direction::input ? virt_in_fr : virt_out_fr).emplace(std::move(port));
            }
        }
    };

    run_while(find_ports());

    if (!virt_in_fl || !virt_in_fr || !virt_out_fl || !virt_out_fr)
    {
        std::println("Could not find all ports for virtual microphone");
        return 1;
    }

    auto links = std::vector<pw::link>{};

    const auto microphone_id = std::format("{}", microphone_node->id());
    const auto speaker_id    = std::format("{}", speaker_node->id());

    const auto create_links = [&]() -> coco::task<void> {
        auto ports = co_await bind_globals(std::type_identity<pw::port>{});

        for (const auto &port : ports)
        {
            auto info = port.info();

            if (!info.props.contains("node.id"))
            {
                continue;
            }

            const auto node    = info.props["node.id"];
            const auto channel = info.props["port.id"];

            // This is a rudimentary check. To do this properly, check the `audio.channel` as well as the device/node's
            // channelMap.

            if (channel != "0" && channel != "1")
            {
                continue;
            }

            auto factory = std::optional<pw::link_factory>{};

            if (node == microphone_id && info.direction == pipewire::port_direction::output)
            {
                factory = {
                    .input  = (channel == "0" ? virt_in_fl : virt_in_fr)->id(),
                    .output = info.id,
                };
            }

            if (node == speaker_id && info.direction == pipewire::port_direction::input)
            {
                factory = {
                    .input  = info.id,
                    .output = (channel == "0" ? virt_out_fl : virt_out_fr)->id(),
                };
            }

            if (!factory)
            {
                continue;
            }

            auto link = co_await core->create(factory.value());

            if (!link)
            {
                std::println("Failed to create link ({} -> {}): {}", factory->input, factory->output, link.error().message);
                continue;
            }

            links.emplace_back(std::move(link.value()));
        }
    };

    run_while(create_links());
    std::cin.get();

    return 0;
}
