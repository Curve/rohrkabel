#include <string>
#include <thread>
#include <iostream>
#include <rohrkabel/main_loop.hpp>
#include <rohrkabel/registry/registry.hpp>

#include <cr/channel.hpp>
#include <rohrkabel/channel/channel.hpp>

struct create_virtual_mic
{
    std::string name;
};

struct terminate
{
};

void main_loop_thread(                                          //
    pipewire::receiver<create_virtual_mic, terminate> receiver, //
    cr::sender<std::string> sender                              //
)
{
    auto main_loop = pipewire::main_loop();
    auto context = pipewire::context(main_loop);
    auto core = pipewire::core(context);
    auto reg = pipewire::registry(core);

    std::vector<pipewire::lazy_expected<pipewire::proxy>> objects;

    receiver.attach(&main_loop, [&](auto &&arg) {
        using arg_t = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<arg_t, create_virtual_mic>)
        {
            objects.emplace_back(core.create("adapter", {{"node.name", arg.name}, {"media.class", "Audio/Source/Virtual"}, {"factory.name", "support.null-audio-sink"}},
                                             pipewire::node::type, pipewire::node::version, pipewire::update_strategy::none));
            sender.send("Node created!");
        }
        else if constexpr (std::is_same_v<arg_t, terminate>)
        {
            main_loop.quit();
        }
    });

    main_loop.run();
}

int main()
{
    auto [main_sender, main_receiver] = cr::channel<std::string>();
    auto [pw_sender, pw_receiver] = pipewire::channel<create_virtual_mic, terminate>();

    std::thread t(main_loop_thread, std::move(pw_receiver), std::move(main_sender));

    pw_sender.send<create_virtual_mic>({"Test Node"});

    std::this_thread::sleep_for(std::chrono::seconds(10));

    main_receiver.receive([](auto &&arg) { std::cout << arg << std::endl; });
    pw_sender.send<terminate>();

    t.join();
    return 0;
}