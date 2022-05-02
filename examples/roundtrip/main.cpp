#include <iostream>
#include "registry/registry.hpp"

int main()
{
    auto main_loop = pipewire::main_loop();
    auto context = pipewire::context(main_loop);
    auto core = pipewire::core(context);
    auto reg = pipewire::registry(core);

    auto reg_listener = reg.listen<pipewire::registry_listener>();
    reg_listener.on<pipewire::registry_event::global>([&](const pipewire::global &global) {
        if (global.type == "PipeWire:Interface:Node")
        {
            auto node = reg.bind<pipewire::node>(global);
            auto info = node.info();

            std::cout << "Node " << info.id << ": ";
            for (const auto &prop : info.props)
            {
                std::cout << "{" << prop.first << ", " << prop.second << "} ";
            }
            std::cout << std::endl;
        }
        if (global.type == "PipeWire:Interface:Metadata")
        {
            auto metadata = reg.bind<pipewire::metadata>(global);

            std::cout << "Metadata: ";
            for (const auto &[key, property] : metadata.properties())
            {
                std::cout << "{" << key << ", " << property.value << "} ";
            }
            std::cout << std::endl;
        }
        if (global.type == "PipeWire:Interface:Port")
        {
            auto port = reg.bind<pipewire::port>(global);
            auto info = port.info();

            std::cout << "Port " << info.id << ": ";
            for (const auto &prop : info.props)
            {
                std::cout << "{" << prop.first << ", " << prop.second << "} ";
            }
            std::cout << std::endl;
        }
    });

    core.sync();
    return 0;
}