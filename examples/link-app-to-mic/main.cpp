#include <iostream>
#include "port/info.hpp"
#include "registry/registry.hpp"

struct linked_port
{
    pipewire::port port;
    pipewire::node &node;
};

int main()
{
    auto main_loop = pipewire::main_loop();
    auto context = pipewire::context(main_loop);
    auto core = pipewire::core(context);
    auto reg = pipewire::registry(core);

    std::string target;
    std::cout << "Enter an application you'd like to link to the virtual microphone: ";

    std::cin >> target;
    std::cout << std::endl;

    auto virtual_mic = core.create("adapter",
                                   {
                                       {"node.name", "Virtual Mic"},                //
                                       {"media.class", "Audio/Source/Virtual"},     //
                                       {"factory.name", "support.null-audio-sink"}, //
                                       {"audio.channels", "2"},                     //
                                       {"audio.position", "FL,FR"}                  //
                                   },
                                   "PipeWire:Interface:Node", 3, false);

    std::map<std::uint32_t, linked_port> ports;
    std::map<std::uint32_t, pipewire::node> nodes;
    std::map<std::uint32_t, pipewire::proxy> links;

    auto reg_events = reg.listen<pipewire::registry_listener>();
    reg_events.on<pipewire::registry_event::global>([&](const pipewire::global &global) {
        if (global.type == "PipeWire:Interface:Node")
        {
            auto node = reg.bind<pipewire::node>(global);
            std::cout << "Added  : " << node.info().props["node.name"] << std::endl;

            if (!nodes.count(global.id))
            {
                nodes.emplace(global.id, std::move(node));
            }
        }
        if (global.type == "PipeWire:Interface:Port")
        {
            auto port = reg.bind<pipewire::port>(global);
            auto info = port.info();

            if (info.props.count("node.id"))
            {
                auto node_id = std::stoll(info.props["node.id"]);

                if (nodes.count(node_id))
                {
                    ports.emplace(global.id, linked_port{std::move(port), nodes.at(node_id)});

                    auto &parent = nodes.at(node_id);
                    if (info.direction == pipewire::port_direction::output && parent.info().props["node.name"].find(target) != std::string::npos)
                    {
                        for (const auto &[port_id, linked_port] : ports)
                        {
                            if (linked_port.node.info().id == virtual_mic.id() && linked_port.port.info().direction == pipewire::port_direction::input)
                            {
                                if (info.props["audio.channel"] == linked_port.port.info().props["audio.channel"])
                                {
                                    links.emplace(info.id, core.create("link-factory",
                                                                       {
                                                                           {"link.input.port", std::to_string(linked_port.port.info().id)}, //
                                                                           {"link.output.port", std::to_string(info.id)}                    //
                                                                       },
                                                                       "PipeWire:Interface:Link", 3));
                                }
                            }
                        }
                    }
                }
            }
        }
    });

    reg_events.on<pipewire::registry_event::global_removed>([&](const std::uint32_t id) {
        if (nodes.count(id))
        {
            auto info = nodes.at(id).info();
            std::cout << "Removed: " << info.props["node.name"] << std::endl;
            nodes.erase(id);
        }
        if (ports.count(id))
        {
            ports.erase(id);
        }
        if (links.count(id))
        {
            links.erase(id);
        }
    });

    while (true)
    {
        main_loop.run();
    }
    return 0;
}