#include <memory>
#include <iostream>
#include <optional>
#include <rohrkabel/loop/main.hpp>
#include <rohrkabel/registry/registry.hpp>
#include <rohrkabel/spa/pod/object/body.hpp>

void search_mute_prop(std::optional<pipewire::spa::pod_prop> &result, const pipewire::spa::pod &pod, pipewire::spa::pod_prop *parent_prop = nullptr);

int main()
{
    auto main_loop = pipewire::main_loop();
    auto context = pipewire::context(main_loop);
    auto core = pipewire::core(context);
    auto reg = pipewire::registry(core);

    std::vector<pipewire::device> devices;

    auto reg_listener = reg.listen<pipewire::registry_listener>();
    reg_listener.on<pipewire::registry_event::global>([&](const pipewire::global &global) {
        if (global.type == pipewire::device::type)
        {
            auto device = reg.bind<pipewire::device>(global.id);
            auto info = device.info();

            if (info.props.count("alsa.card_name"))
            {
                devices.emplace_back(std::move(device));
            }
        }
    });
    core.update();

    for (auto i = 0u; devices.size() > i; i++)
    {
        auto &device = devices.at(i);
        auto name = device.info().props.at("alsa.card_name");

        std::cout << i << ": " << name << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Select a device to mute: ";

    std::size_t selection{0};
    std::cin >> selection;

    auto &device = devices.at(selection);

    std::cout << std::endl;
    std::cout << "Muting: " << device.info().props.at("alsa.card_name") << std::endl;

    auto params = device.params();
    core.update();

    for (const auto &[pod_id, pod] : params.get())
    {
        std::optional<pipewire::spa::pod_prop> result;
        search_mute_prop(result, pod);

        if (result)
        {
            std::cout << "Mute-Prop: " << result->name() << " (" << pod_id << ") [" << result->key() << "]" << std::endl;
            result->value().write(!result->value().as<bool>());

            device.set_param(pod_id, pod.get());
            core.update();

            std::cout << "Device muted!" << std::endl;
            return 0;
        }
    }

    std::cout << "Could not find mute prop for device!" << std::endl;
    return 1;

    //? Instead of enumerating all pods you could also use the short version:
    /*
        auto mute = pods.at(13).body<pipewire::spa::pod_object_body>().at(10).value().body<pipewire::spa::pod_object_body>().at(65540).value();

        mute.as<bool>() = !mute.as<bool>();
        device.set_param(13, pods.at(13).get());

        core.sync();
    */
}

// NOLINTNEXTLINE
void search_mute_prop(std::optional<pipewire::spa::pod_prop> &result, const pipewire::spa::pod &pod, pipewire::spa::pod_prop *parent_prop)
{
    if (pod.type() == pipewire::spa::pod_type::object)
    {
        for (auto prop : pod.body<pipewire::spa::pod_object_body>())
        {
            search_mute_prop(result, prop.value(), &prop);
        }
    }
    if (pod.type() == pipewire::spa::pod_type::boolean)
    {
        if (parent_prop && parent_prop->name().find("mute") != std::string::npos)
        {
            result.emplace(std::move(*parent_prop));
        }
    }
}