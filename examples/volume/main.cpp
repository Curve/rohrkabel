#include <cmath>
#include <format>
#include <iostream>

#include <rohrkabel/node/node.hpp>
#include <rohrkabel/spa/pod/object/body.hpp>

#include <rohrkabel/registry/events.hpp>
#include <rohrkabel/registry/registry.hpp>

namespace pw = pipewire;

int main()
{
    auto main_loop = pw::main_loop::create();
    auto context   = pipewire::context::create(main_loop);
    auto core      = pw::core::create(context);
    auto reg       = pw::registry::create(core);

    std::vector<pw::node> devices;

    auto listener = reg->listen();

    auto on_global = [&](const pipewire::global &global) {
        if (global.type != pipewire::node::type)
        {
            return;
        }

        auto device = reg->bind<pipewire::node>(global.id).get();
        auto props  = device->info().props;

        if (!props.contains("node.nick"))
        {
            return;
        }

        if (props["media.class"].find("Audio") == std::string::npos)
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

        auto name  = device.info().props.at("node.nick");
        auto media = device.info().props.at("media.class");

        std::cout << std::format("{}. {} ({})", i, name, media) << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Select a device to change the volume of: ";

    std::size_t selection{0};
    std::cin >> selection;

    auto &device = devices.at(selection);
    std::cout << "Input new volume for '" << device.info().props.at("node.nick") << "': ";

    float volume{0};
    std::cin >> volume;
    std::cout << std::endl;

    auto params = device.params();
    core->update();

    auto get_channels = [](const pw::spa::pod &pod) {
        // NOLINTNEXTLINE
        auto impl = [](const pw::spa::pod_prop *parent, const pw::spa::pod &pod,
                       auto &self) -> std::optional<pw::spa::pod_prop> {
            if (pod.type() == pw::spa::pod_type::object)
            {
                for (const auto &item : pod.body<pw::spa::pod_object_body>())
                {
                    auto rtn = self(&item, item.value(), self);

                    if (!rtn.has_value())
                    {
                        continue;
                    }

                    return rtn;
                }
            }

            if (!parent || !parent->name().ends_with("channelVolumes"))
            {
                return std::nullopt;
            }

            return *parent;
        };

        return impl(nullptr, pod, impl);
    };

    for (const auto &[pod_id, pod] : params.get())
    {
        auto channels = get_channels(pod);

        if (!channels)
        {
            continue;
        }

        // pipewire uses cubic volumes! (that's why we use std::cbrt, and std::pow)

        std::cout << std::format("Channels: {} ({}) [{}]", channels->name(), pod_id, channels->key()) << std::endl;

        auto volumes = channels->value().read<std::vector<float>>();
        std::cout << std::format("Changed volume from {}% to {}%", std::cbrt(volumes[0]) * 100, volume) << std::endl;

        auto new_volumes = volumes | std::views::transform([volume](auto &&...) {
                               return std::pow(volume / 100, 3);
                           });

        channels->value().write<std::vector<float>>({new_volumes.begin(), new_volumes.end()});

        device.set_param(pod_id, 0, pod);
        core->update();

        return 0;
    }

    std::cout << "Could not find channels for node!" << std::endl;
    return 1;
}
