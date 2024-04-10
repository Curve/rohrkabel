#include <format>
#include <iostream>

#include <rohrkabel/device/device.hpp>
#include <rohrkabel/spa/pod/object/body.hpp>

#include <rohrkabel/registry/events.hpp>
#include <rohrkabel/registry/registry.hpp>

namespace pw = pipewire;

int main()
{
    auto main_loop = pw::main_loop::create();
    auto context   = pipewire::context::create(main_loop);
    auto core      = context->core();
    auto reg       = core->registry();

    std::vector<pw::device> devices;

    auto listener = reg->listen();

    auto on_global = [&](const pipewire::global &global) {
        if (global.type != pipewire::device::type)
        {
            return;
        }

        auto device = reg->bind<pipewire::device>(global.id).get();
        auto info   = device->info();

        if (!info.props.contains("alsa.card_name"))
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
        auto name    = device.info().props.at("alsa.card_name");

        std::cout << std::format("{}. {}", i, name) << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Select a device to mute: ";

    std::size_t selection{0};
    std::cin >> selection;

    auto &device = devices.at(selection);

    std::cout << std::endl;
    std::cout << "Muting: " << device.info().props.at("alsa.card_name") << std::endl;

    auto params = device.params();
    core->update();

    auto get_mute = [](const pw::spa::pod &pod) {
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

            if (parent && pod.type() == pw::spa::pod_type::boolean && parent->name().find("mute") != std::string::npos)
            {
                return *parent;
            }

            return std::nullopt;
        };

        return impl(nullptr, pod, impl);
    };

    for (const auto &[pod_id, pod] : params.get())
    {
        auto mute = get_mute(pod);

        if (!mute)
        {
            continue;
        }

        std::cout << std::format("Mute-Prop: {} ({}) [{}]", mute->name(), pod_id, mute->key()) << std::endl;
        mute->value().write(!mute->value().read<bool>());

        device.set_param(pod_id, 0, pod);
        core->update();

        std::cout << "Device muted!" << std::endl;
        return 0;
    }

    std::cout << "Could not find mute prop for device!" << std::endl;
    return 1;
}

//? Instead of enumerating all pods you could also use the short version:
/*
    auto mute =
   pods.at(13).body<pipewire::spa::pod_object_body>().at(10).value().body<pipewire::spa::pod_object_body>().at(65540).value();

    mute.as<bool>() = !mute.as<bool>();
    device.set_param(13, pods.at(13).get());

    core.sync();
*/
