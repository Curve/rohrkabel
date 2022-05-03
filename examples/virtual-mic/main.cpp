#include <iostream>
#include "registry/registry.hpp"

int main()
{
    auto main_loop = pipewire::main_loop();
    auto context = pipewire::context(main_loop);
    auto core = pipewire::core(context);
    auto reg = pipewire::registry(core);

    std::vector<pipewire::port> ports;
    std::string default_sink, default_source;
    std::map<std::uint32_t, pipewire::node> nodes;

    auto metadata_listener = reg.listen<pipewire::registry_listener>();
    metadata_listener.on<pipewire::registry_event::global>([&](const pipewire::global &global) {
        if (global.type == "PipeWire:Interface:Metadata")
        {
            auto metadata = reg.bind<pipewire::metadata>(global);
            auto properties = metadata.properties();

            if (properties.count("default.audio.sink"))
            {
                auto value = properties.at("default.audio.sink").value;

                auto start = value.find_first_of(':') + 2;
                auto len = value.find_last_of('"') - start;

                auto name = value.substr(start, len);
                default_sink = name;
            }

            if (properties.count("default.audio.source"))
            {
                auto value = properties.at("default.audio.source").value;

                auto start = value.find_first_of(':') + 2;
                auto len = value.find_last_of('"') - start;

                auto name = value.substr(start, len);
                default_source = name;
            }
        }
        if (global.type == "PipeWire:Interface:Node")
        {
            nodes.emplace(global.id, reg.bind<pipewire::node>(global));
        }
        if (global.type == "PipeWire:Interface:Port")
        {
            ports.emplace_back(reg.bind<pipewire::port>(global));
        }
    });
    core.sync();

    std::uint32_t mic_node{0}, speaker_node{0};
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

    auto virtual_mic =
        core.create("adapter", {{"node.name", "Virtual Mic"}, {"media.class", "Audio/Source/Virtual"}, {"factory.name", "support.null-audio-sink"}}, "PipeWire:Interface:Node", 3);

    //? The Ports are available shortly after the virtual microphone has been created
    while (ports.size() == old_port_size)
    {
        core.sync();
    }

    const pipewire::port *virt_in_fr{}, *virt_in_fl{};
    for (const auto &port : ports)
    {
        auto info = port.info();
        auto node_id = std::stoll(info.props["node.id"]);

        if (node_id == virtual_mic.id() && info.direction == pipewire::port_direction::input)
        {
            auto audio_channel = info.props["audio.channel"];
            if (audio_channel == "FL")
            {
                virt_in_fl = &port;
            }
            else if (audio_channel == "FR")
            {
                virt_in_fr = &port;
            }
        }
    }

    if (!virt_in_fl || !virt_in_fr)
    {
        std::cerr << "This program currently only works with Stereo (Or Above) Speakers & Microphones" << std::endl;
        return 1;
    }

    std::vector<pipewire::proxy> links;
    for (const auto &port : ports)
    {
        auto info = port.info();

        if (info.props.count("node.id") && info.direction == pipewire::port_direction::output)
        {
            auto node_id = std::stoll(info.props.at("node.id"));
            auto parent_node = nodes.at(node_id).info();

            if (parent_node.id == mic_node || parent_node.id == speaker_node)
            {
                if (info.props["audio.channel"] == "FL")
                {
                    links.emplace_back(core.create(
                        "link-factory", {{"application.name", "link"}, {"link.input.port", std::to_string(virt_in_fl->info().id)}, {"link.output.port", std::to_string(info.id)}},
                        "PipeWire:Interface:Link", 3));
                }
                else if (info.props["audio.channel"] == "FR")
                {
                    links.emplace_back(core.create(
                        "link-factory", {{"application.name", "link"}, {"link.input.port", std::to_string(virt_in_fr->info().id)}, {"link.output.port", std::to_string(info.id)}},
                        "PipeWire:Interface:Link", 3));
                }
            }
        }
    }

    std::cin.get();
    return 0;
}