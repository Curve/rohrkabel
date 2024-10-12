#include <format>
#include <iostream>

#include <rohrkabel/device/device.hpp>
#include <rohrkabel/spa/pod/object/object.hpp>

#include <rohrkabel/registry/events.hpp>
#include <rohrkabel/registry/registry.hpp>

namespace pw = pipewire;

int main()
{
    auto main_loop = pw::main_loop::create();
    auto context   = pipewire::context::create(main_loop);
    auto core      = pw::core::create(context);
    auto reg       = pw::registry::create(core);

    std::vector<pw::device> devices;

    auto listener = reg->listen();

    auto on_global = [&](const pipewire::global &global) {
        if (global.type != pipewire::device::type)
        {
            return;
        }

        auto device = reg->bind<pipewire::device>(global.id).get();
        auto info   = device->info();

        if (info.props["media.class"] != "Audio/Device")
        {
            return;
        }

        if (!info.props.contains("device.description"))
        {
            return;
        }

        devices.emplace_back(std::move(*device));
    };

    listener.on<pipewire::registry_event::global>(on_global);
    core->update();

    for (auto i = 0u; devices.size() > i; i++)
    {
        auto &device = devices.at(i);
        auto name    = device.info().props.at("device.description");

        std::cout << std::format("{}. {}", i, name) << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Select a device to mute: ";

    std::size_t selection{0};
    std::cin >> selection;

    auto &device = devices.at(selection);

    std::cout << std::endl;
    std::cout << "Muting: " << device.info().props.at("device.description") << std::endl;

    auto params = device.params();
    core->update();

    for (const auto &[pod_id, pod] : params.get())
    {
        auto prop = pod.find_recursive(pw::spa::prop::mute);

        if (!prop)
        {
            continue;
        }

        prop->value().write(!prop->value().as<bool>());
        device.set_param(pod_id, 0, pod);
        core->update();

        std::cout << "Device muted!" << std::endl;
        return 0;
    }

    std::cout << "Could not find mute prop for device!" << std::endl;
    return 1;
}
