#include <iostream>
#include <optional>
#include <rohrkabel/loop/thread.hpp>
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
    auto loop = pw::thread_loop("example");
    auto context = pw::context(loop);
    auto core = pw::core(context);
    auto reg = std::make_unique<pw::registry>(core);

    loop.run();

    std::optional<pipewire::proxy> virt_mic;
    {
        std::lock_guard guard(loop);
        virt_mic.emplace(core.create("adapter", //
                                     {
                                         {"node.name", "Virtual Mic"},               //
                                         {"media.class", "Audio/Source/Virtual"},    //
                                         {"factory.name", "support.null-audio-sink"} //
                                     },
                                     pw::node::type, pw::node::version));
    }
    core.update();

    std::optional<pipewire::node> virt_mic_node;
    {
        std::lock_guard guard(loop);
        virt_mic_node.emplace(reg->bind<pw::node>(virt_mic->id()));
    }
    core.update();

    loop.lock();
    auto params = virt_mic_node->params();
    loop.unlock();

    core.update();

    std::cout << "Virt Mic: " << virt_mic->id() << std::endl;

    for (const auto &pod : params.get())
    {
        std::cout << pod.first << " - ";
        dump(pod.second);
    }

    loop.quit();
    return 0;
}