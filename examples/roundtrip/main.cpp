#include <iostream>
#include <rohrkabel/loop/main.hpp>
#include <rohrkabel/registry/registry.hpp>

int main()
{
    auto main_loop = pipewire::main_loop();
    auto context = pipewire::context(main_loop);
    auto core = pipewire::core(context);
    auto reg = pipewire::registry(core);

    auto reg_listener = reg.listen<pipewire::registry_listener>();
    reg_listener.on<pipewire::registry_event::global>([&](const pipewire::global &global) {
        if (global.type == pipewire::node::type)
        {
            auto node = reg.bind<pipewire::node>(global.id);
            auto info = node.info();

            std::cout << "Node " << info.id << ": ";
            for (const auto &prop : info.props)
            {
                std::cout << "{" << prop.first << ", " << prop.second << "} ";
            }
            std::cout << std::endl;
        }
        if (global.type == pipewire::metadata::type)
        {
            auto metadata = reg.bind<pipewire::metadata>(global.id);

            std::cout << "Metadata: ";
            for (const auto &[key, property] : metadata.properties())
            {
                std::cout << "{" << key << ", " << property.value << "} ";
            }
            std::cout << std::endl;
        }
        if (global.type == pipewire::port::type)
        {
            auto port = reg.bind<pipewire::port>(global.id);
            auto info = port.info();

            std::cout << "Port " << info.id << ": ";
            for (const auto &prop : info.props)
            {
                std::cout << "{" << prop.first << ", " << prop.second << "} ";
            }
            std::cout << std::endl;
        }
        if (global.type == pipewire::device::type)
        {
            auto device = reg.bind<pipewire::device>(global.id);
            auto info = device.info();

            std::cout << "Device " << info.id << ": ";
            for (const auto &prop : info.props)
            {
                std::cout << "{" << prop.first << ", " << prop.second << "} ";
            }
            std::cout << std::endl;
        }
        if (global.type == pipewire::client::type)
        {
            auto client = reg.bind<pipewire::client>(global.id);
            auto info = client.info();

            std::cout << "Client " << info.id << ": ";
            for (const auto &prop : info.props)
            {
                std::cout << "{" << prop.first << ", " << prop.second << "} ";
            }
            std::cout << std::endl;
        }
    });

    core.update();
    return 0;
}