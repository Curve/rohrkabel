#include "rohrkabel/core/core.hpp"
#include "rohrkabel/spa/pod/pod.hpp"
#include <thread>
#include <iostream>
#include <rohrkabel/loop/main.hpp>
#include <rohrkabel/registry/registry.hpp>
#include <rohrkabel/spa/pod/object/body.hpp>

namespace pw = pipewire;

void dump(const pw::spa::pod &pod, std::size_t I = 0)
{
    if (pod.type() == pw::spa::pod_type::object)
    {
        std::cout << std::string(I, '\t') << "Object: " << pod.name() << std::endl;
        I++;

        for (const auto &entry : pod.body<pw::spa::pod_object_body>())
        {
            std::cout << std::string(I, '\t') << entry.name() << ": " << std::endl;
            dump(entry.value(), I + 1);
        }
    }
    else if (pod.type() == pw::spa::pod_type::boolean)
    {
        std::cout << std::string(I, '\t') << pod.name() << " => " << pod.as<bool>() << std::endl;
    }
    else if (pod.type() == pw::spa::pod_type::num_float)
    {
        std::cout << std::string(I, '\t') << pod.name() << " => " << pod.as<float>() << std::endl;
    }
    else
    {
        std::cout << std::string(I, '\t') << " - " << pod.name() << " (" << static_cast<int>(pod.type()) << ")" << std::endl;
    }
}

int main()
{
    auto loop = pw::main_loop();
    auto context = pw::context(loop);
    auto core = pw::core(context);
    auto reg = std::make_unique<pw::registry>(core);

    // ? Run the main-loop in a different thread
    std::thread t1([&] { loop.run(); });

    auto virt_mic = loop.call_safe([&] {
        return core.create("adapter", //
                           {
                               {"node.name", "Virtual Mic"},               //
                               {"media.class", "Audio/Source/Virtual"},    //
                               {"factory.name", "support.null-audio-sink"} //
                           },
                           pw::node::type,           //
                           pw::node::version,        //
                           pw::update_strategy::none //? Don't update automatically to not cause the main-loop to quit
        );
    });

    core.update(); // ! Make sure we update before accessing anything
    std::cout << "Virt Mic: " << virt_mic->id() << std::endl;

    auto virt_mic_node = loop.call_safe([&reg, &virt_mic] { return reg->bind<pw::node>(virt_mic->id(), pw::update_strategy::none); });
    core.update();

    auto virt_mic_params = loop.call_safe([&] { return virt_mic_node.get_safe().params(); });
    core.update(); // ! Make sure to update after receiving the params, otherwise they will be unset

    for (const auto &pod : virt_mic_params.get())
    {
        std::cout << pod.first << " - ";
        dump(pod.second);
    }

    loop.call_safe([&] { loop.quit(); });
    t1.join();

    return 0;
}