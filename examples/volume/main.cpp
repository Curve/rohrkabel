#include <print>
#include <iostream>

#include <cmath>

#include <rohrkabel/device/device.hpp>
#include <rohrkabel/spa/pod/object.hpp>

#include <rohrkabel/registry/events.hpp>
#include <rohrkabel/registry/registry.hpp>

namespace pw = pipewire;

int main()
{
    auto loop    = pw::main_loop::create();
    auto context = pipewire::context::create(loop);
    auto core    = pw::core::create(context);
    auto reg     = pw::registry::create(core);

    auto listener = reg->listen();
    auto devices  = std::vector<pw::device>{};

    auto on_global = [&](const pw::global &global)
    {
        if (global.type != pw::device::type)
        {
            return;
        }

        auto device = core->await(reg->bind<pw::device>(global.id));

        if (!device.has_value())
        {
            return;
        }

        auto info = device->info();

        if (info.props["media.class"] != "Audio/Device")
        {
            return;
        }

        if (!info.props.contains("device.description"))
        {
            return;
        }

        devices.emplace_back(std::move(device.value()));
    };

    listener.on<pipewire::registry_event::global>(on_global);
    core->run_once();

    for (auto i = 0u; devices.size() > i; i++)
    {
        auto &device = devices.at(i);
        auto name    = device.info().props.at("device.description");
        std::println("{}. {}", i, name);
    }

    std::print("\nSelect a device to change the volume of: ");

    std::size_t selection{0};
    std::cin >> selection;

    auto &device = devices.at(selection);
    std::print("Input new volume for '{}': ", device.info().props.at("device.description"));

    float volume{0};
    std::cin >> volume;

    std::println();

    auto params = core->await(device.params());

    for (const auto &[pod_id, pod] : params)
    {
        auto prop = pod.find_recursive(pw::spa::prop::channel_volumes);

        if (!prop)
        {
            continue;
        }

        // pipewire uses cubic volumes! (that's why we use std::cbrt, and std::pow)

        auto channels      = prop->value().as<std::vector<float>>();
        auto cubic_volumes = channels | std::views::transform(
                                            [volume](auto &&)
                                            {
                                                return std::powf(volume / 100, 3);
                                            });

        prop->value().write<std::vector<float>>({cubic_volumes.begin(), cubic_volumes.end()});
        device.set_param(pod_id, 0, pod);

        core->run_once();
        std::cout << std::format("Updated volume from {}% to {}%", std::cbrt(channels[0]) * 100, volume) << std::endl;

        return 0;
    }

    std::println("Could not find volume prop for device!");

    return 1;
}
