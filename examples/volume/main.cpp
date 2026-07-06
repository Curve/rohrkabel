#include <print>
#include <iostream>

#include <cmath>

#include <rohrkabel/device/device.hpp>
#include <rohrkabel/spa/pod/object.hpp>

#include <rohrkabel/registry/events.hpp>
#include <rohrkabel/registry/registry.hpp>

namespace pw = pipewire;

pw::lazy<int> co_main(std::shared_ptr<pw::main_loop> &loop, std::shared_ptr<pw::core> &core) // NOLINT(*-coroutine-parameters)
{
    const auto exit = [&](auto &&value)
    {
        loop->quit();
        return value;
    };

    auto reg      = pw::registry::create(core);
    auto listener = reg->listen();
    auto devices  = std::vector<pw::device>{};

    auto on_global = [&](pw::global global) -> coco::stray // NOLINT(*-lambda-coroutines)
    {
        if (global.type != pw::device::type)
        {
            co_return;
        }

        auto device = co_await reg->bind<pw::device>(global.id);

        if (!device.has_value())
        {
            co_return;
        }

        auto info = device->info();

        if (info.props["media.class"] != "Audio/Device")
        {
            co_return;
        }

        if (!info.props.contains("device.description"))
        {
            co_return;
        }

        devices.emplace_back(std::move(device.value()));
    };

    listener.on<pipewire::registry_event::global>(on_global);
    co_await core->sync<pw::sync_mode::recursive>(); // Wait for reg->bind to resolve as well...

    for (auto i = 0u; devices.size() > i; i++)
    {
        auto &device = devices.at(i);
        auto name    = device.info().props.at("device.description");
        std::println("{}. {}", i, name);
    }

    std::print("\nSelect a device to change the volume of: ");

    std::size_t selection{0};
    std::cin >> selection;
    std::println();

    auto &device = devices.at(selection);
    std::print("Input new volume for '{}': ", device.info().props.at("device.description"));

    float volume{0};
    std::cin >> volume;
    std::println();

    auto params = co_await core->await(device.params());

    for (const auto &[pod_id, pod] : params)
    {
        auto prop = pod.find_recursive(pw::spa::prop::channel_volumes);

        if (!prop)
        {
            continue;
        }

        // pipewire uses cubic volumes! (that's why we use std::cbrt, and std::pow)

        auto channels      = prop->value().read<std::vector<float>>();
        auto cubic_volumes = channels | std::views::transform(
                                            [volume](auto &&)
                                            {
                                                return std::powf(volume / 100, 3);
                                            });

        prop->value().write<std::vector<float>>({cubic_volumes.begin(), cubic_volumes.end()});
        device.set_param(pod_id, 0, pod);
        co_await core->sync();

        std::cout << std::format("Updated volume from {:.2}% to {:.2}%", std::cbrt(channels[0]) * 100, volume) << std::endl;

        co_return exit(0);
    }

    std::println("Could not find volume prop for device!");

    co_return exit(1);
}

int main()
{
    auto loop    = pw::main_loop::create().value();
    auto context = pipewire::context::create(loop).value();
    auto core    = pw::core::create(context).value();

    auto result = co_main(loop, core);
    loop->run();

    return coco::await(std::move(result));
}
