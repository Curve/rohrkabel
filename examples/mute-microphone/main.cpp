#include <print>
#include <iostream>

#include <rohrkabel/device/device.hpp>
#include <rohrkabel/spa/pod/object.hpp>

#include <rohrkabel/registry/events.hpp>
#include <rohrkabel/registry/registry.hpp>

namespace pw = pipewire;

int main()
{
    auto loop    = pw::main_loop::create();
    auto context = pw::context::create(loop);
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

        if (!device->info().props.contains("device.description"))
        {
            return;
        }

        if (device->info().props["media.class"] != "Audio/Device")
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
        std::println("{}. {}", i, device.info().props.at("device.description"));
    }

    std::print("\nSelect a device to mute: ");

    std::size_t selection{0};
    std::cin >> selection;

    auto &device = devices.at(selection);
    std::println("\nMuting: {}", device.info().props.at("device.description"));

    auto params = core->await(device.params());

    for (const auto &[pod_id, pod] : params)
    {
        auto prop = pod.find_recursive(pw::spa::prop::mute);

        if (!prop)
        {
            continue;
        }

        auto previous = prop->value().read<bool>();

        prop->value().write(!previous);
        device.set_param(pod_id, 0, pod);

        core->run_once();
        std::println("Device {}!", previous ? "Unmuted" : "Muted");

        return 0;
    }

    std::println("Could not find mute prop!");

    return 1;
}
