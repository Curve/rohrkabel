#include <cmath>
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
    std::cout << "Select a device to change the volume of: ";

    std::size_t selection{0};
    std::cin >> selection;

    auto &device = devices.at(selection);
    std::cout << "Input new volume for '" << device.info().props.at("device.description") << "': ";

    float volume{0};
    std::cin >> volume;
    std::cout << std::endl;

    auto params = device.params();
    core->update();

    for (const auto &[pod_id, pod] : params.get())
    {
        auto prop = pod.find_recursive(pw::spa::prop::channel_volumes);

        if (!prop)
        {
            continue;
        }

        // pipewire uses cubic volumes! (that's why we use std::cbrt, and std::pow)

        auto channels      = prop->value().as<std::vector<float>>();
        auto cubic_volumes = channels | std::views::transform([volume](auto &&) {
                                 return std::powf(volume / 100, 3);
                             });

        std::cout << std::format("Updating volume from {}% to {}%", std::cbrt(channels[0]) * 100, volume) << std::endl;

        prop->value().write<std::vector<float>>({cubic_volumes.begin(), cubic_volumes.end()});
        device.set_param(pod_id, 0, pod);
        core->update();

        return 0;
    }

    std::cout << "Could not find volume prop for device!" << std::endl;
    return 1;
}
