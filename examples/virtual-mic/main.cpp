#include "rohrkabel/port/info.hpp"
#include <iostream>

#include <rohrkabel/port/port.hpp>
#include <rohrkabel/link/link.hpp>
#include <rohrkabel/node/node.hpp>

#include <rohrkabel/metadata/metadata.hpp>

#include <rohrkabel/registry/events.hpp>
#include <rohrkabel/registry/registry.hpp>

namespace pw = pipewire;

int main()
{
    auto main_loop = pw::main_loop::create();
    auto context   = pw::context::create(main_loop);
    auto core      = context->core();
    auto reg       = core->registry();

    std::vector<pw::port> ports;
    std::map<std::uint32_t, pipewire::node> nodes;

    std::string default_sink;
    std::string default_source;

    auto listener = reg->listen();

    auto on_global = [&](const pw::global &global) {
        if (global.type == pw::metadata::type)
        {
            auto metadata   = reg->bind<pw::metadata>(global.id).get();
            auto properties = metadata->properties();

            if (properties.contains("default.audio.sink"))
            {
                auto value = properties.at("default.audio.sink").value;

                auto start = value.find_first_of(':') + 2;
                auto len   = value.find_last_of('"') - start;

                auto name    = value.substr(start, len);
                default_sink = name;
            }

            if (properties.contains("default.audio.source"))
            {
                auto value = properties.at("default.audio.source").value;

                auto start = value.find_first_of(':') + 2;
                auto len   = value.find_last_of('"') - start;

                auto name      = value.substr(start, len);
                default_source = name;
            }
        }

        if (global.type == pw::node::type)
        {
            nodes.emplace(global.id, *reg->bind<pw::node>(global.id).get());
        }

        if (global.type == pw::port::type)
        {
            ports.emplace_back(*reg->bind<pw::port>(global.id).get());
        }
    };

    listener.on<pipewire::registry_event::global>(on_global);
    core->update();

    std::uint32_t mic_node{0};
    std::uint32_t speaker_node{0};

    for (const auto &[id, node] : nodes)
    {
        auto info = node.info();
        auto name = info.props["node.name"];

        if (name == default_sink)
        {
            speaker_node = id;
        }

        if (name == default_source)
        {
            mic_node = id;
        }
    }

    auto old_port_size = ports.size();

    auto virtual_mic = core->create<pw::node>({
                                                  .name = "adapter",
                                                  .props =
                                                      {
                                                          {"node.name", "Virtual Mic"},
                                                          {"media.class", "Audio/Source/Virtual"},
                                                          {"factory.name", "support.null-audio-sink"},
                                                      },
                                              })
                           .get();

    //? The Ports are available shortly after the virtual microphone has been created

    while (ports.size() == old_port_size)
    {
        core->update();
    }

    const pipewire::port *virt_in_fr{};
    const pipewire::port *virt_in_fl{};

    const pipewire::port *virt_out_fr{};
    const pipewire::port *virt_out_fl{};

    for (const auto &port : ports)
    {
        auto info    = port.info();
        auto node_id = std::stoll(info.props["node.id"]);

        if (node_id != virtual_mic->id())
        {
            continue;
        }

        auto audio_channel = info.props["audio.channel"];

        if (audio_channel == "FL")
        {
            (info.direction == pw::port_direction::input ? virt_in_fl : virt_out_fl) = &port;
        }
        else if (audio_channel == "FR")
        {
            (info.direction == pw::port_direction::input ? virt_in_fr : virt_out_fr) = &port;
        }
    }

    if (!virt_in_fl || !virt_in_fr)
    {
        std::cerr << "This program currently only works with Stereo (Or Above) Speakers & Microphones" << std::endl;
        return 1;
    }

    std::vector<pw::link> links;

    for (const auto &port : ports)
    {
        auto info = port.info();

        if (!info.props.contains("node.id"))
        {
            continue;
        }

        auto node_id = std::stoll(info.props.at("node.id"));
        auto node    = nodes.at(node_id).info();

        auto channel = info.props["audio.channel"];

        if (channel != "FL" && channel != "FR")
        {
            continue;
        }

        if (node.id == mic_node && info.direction == pipewire::port_direction::output)
        {
            links.emplace_back(
                *core->create<pw::link>(pw::link_factory{(channel == "FL" ? virt_in_fl : virt_in_fr)->id(), info.id})
                     .get());
        }

        if (node.id == speaker_node && info.direction == pipewire::port_direction::input)
        {
            links.emplace_back(
                *core->create<pw::link>(pw::link_factory{info.id, (channel == "FR" ? virt_out_fl : virt_out_fr)->id()})
                     .get());
        }
    }

    std::cin.get();
    return 0;
}
