#include <print>
#include <iostream>

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

    auto listener   = reg->listen();
    auto microphone = std::string{};
    auto speaker    = std::string{};

    auto nodes = std::vector<pw::node>{};
    auto ports = std::vector<pw::port>{};

    const auto on_global = [&](const pw::global &global)
    {
        if (global.type == pw::metadata::type)
        {
            auto metadata = core->await(reg->bind<pw::metadata>(global.id));

            auto props      = metadata->props();
            auto properties = metadata->properties();

            if (props["metadata.name"] != "default")
            {
                return;
            }

            speaker    = extract_name(properties["default.audio.sink"].value);
            microphone = extract_name(properties["default.audio.source"].value);
        }

        if (global.type == pw::node::type)
        {
            auto node = core->await(reg->bind<pw::node>(global.id));

            if (!node)
            {
                return;
            }

            nodes.emplace_back(std::move(node.value()));
        }

        if (global.type == pw::port::type)
        {
            auto port = core->await(reg->bind<pw::port>(global.id));

            if (!port)
            {
                return;
            }

            ports.emplace_back(std::move(port.value()));
        }
    };

    listener.on<pw::registry_event::global>(on_global);
    core->run_once();

    if (microphone.empty() || speaker.empty())
    {
        std::println("Failed to parse default devices from metadata");
        return 1;
    }

    const auto is = [&](auto what)
    {
        return [what](auto &&x)
        {
            auto props = x.props();
            return props["node.name"] == what;
        };
    };

    auto speaker_node    = std::optional<pw::node>{};
    auto microphone_node = std::optional<pw::node>{};

    if (auto it = std::ranges::find_if(nodes, is(speaker)); it != nodes.end())
    {
        speaker_node.emplace(std::move(*it));
        nodes.erase(it);
    }

    if (auto it = std::ranges::find_if(nodes, is(microphone)); it != nodes.end())
    {
        microphone_node.emplace(std::move(*it));
        nodes.erase(it);
    }

    if (!speaker_node || !microphone_node)
    {
        std::println("Could not find speaker or microphone node");
        return 1;
    }

    auto virt_mic = core->await(core->create(pw::null_sink_factory{
        .name      = "Virtual Mic",
        .positions = {"FL", "FR"},
    }));

    if (!virt_mic)
    {
        std::println("Failed to create null sink: {}", virt_mic.error().message);
        return 1;
    }

    const auto prev_ports = ports.size();

    // The port creation of the null-sink is slightly delayed.
    while (ports.size() == prev_ports)
    {
        core->run_once();
    }

    const auto virt_mic_id = std::format("{}", virt_mic->id());

    auto virt_in_fl = std::optional<pw::port>{};
    auto virt_in_fr = std::optional<pw::port>{};

    auto virt_out_fl = std::optional<pw::port>{};
    auto virt_out_fr = std::optional<pw::port>{};

    for (auto it = ports.begin(); it != ports.end();)
    {
        auto &port = *it;
        auto props = port.props();

        if (props["node.id"] != virt_mic_id)
        {
            ++it;
            continue;
        }

        const auto channel = props["audio.channel"];
        const auto info    = port.info();

        if (channel == "FL")
        {
            (info.direction == pw::port_direction::input ? virt_in_fl : virt_out_fl).emplace(std::move(port));
            it = ports.erase(it);
            continue;
        }

        if (channel == "FR")
        {
            (info.direction == pw::port_direction::input ? virt_in_fr : virt_out_fr).emplace(std::move(port));
            it = ports.erase(it);
            continue;
        }

        ++it;
    }

    if (!virt_in_fl || !virt_in_fr || !virt_out_fl || !virt_out_fr)
    {
        std::println("Could not find all ports for virtual microphone");
        return 1;
    }

    auto links = std::vector<pw::link>{};

    const auto microphone_id = std::format("{}", microphone_node->id());
    const auto speaker_id    = std::format("{}", speaker_node->id());

    for (const auto &port : ports)
    {
        auto info = port.info();

        if (!info.props.contains("node.id"))
        {
            continue;
        }

        const auto node    = info.props["node.id"];
        const auto port_id = info.props["port.id"];

        // This is a rudimentary check. To do this properly, check the `audio.channel` as well as the device/node's
        // channelMap.

        if (port_id != "0" && port_id != "1")
        {
            continue;
        }

        auto factory = std::optional<pw::link_factory>{};

        if (node == microphone_id && info.direction == pipewire::port_direction::output)
        {
            factory = {
                .input  = (port_id == "0" ? virt_in_fl : virt_in_fr)->id(),
                .output = info.id,
            };
        }

        if (node == speaker_id && info.direction == pipewire::port_direction::input)
        {
            factory = {
                .input  = info.id,
                .output = (port_id == "0" ? virt_out_fl : virt_out_fr)->id(),
            };
        }

        if (!factory)
        {
            continue;
        }

        auto link = core->await(core->create(factory.value()));

        if (!link)
        {
            std::println("Failed to create link ({} -> {}): {}", factory->input, factory->output, link.error().message);
            continue;
        }

        links.emplace_back(std::move(link.value()));
        std::println("Created link: [{:<3}] -> [{:<3}]", factory->input, factory->output);
    }

    std::cin.get();

    return 0;
}
