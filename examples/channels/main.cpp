#include <thread>
#include <iostream>

#include <rohrkabel/node/node.hpp>
#include <rohrkabel/channel/channel.hpp>
#include <rohrkabel/registry/registry.hpp>

namespace pw = pipewire;

struct create_virtual_mic
{
    std::string name;
};

struct check_node
{
};

struct terminate
{
};

using recipe = pw::recipe<create_virtual_mic, check_node, terminate>;

int main()
{
    auto [sender, receiver] = pw::channel<recipe>();

    auto thread = [](recipe::receiver receiver)
    {
        auto loop    = pw::main_loop::create();
        auto context = pw::context::create(loop);
        auto core    = context->core();
        auto reg     = core->registry();

        std::vector<pw::expected<pw::node>> created;

        bool exit = false;

        receiver.attach(loop,
                        [&]<typename T>(const T &msg)
                        {
                            if constexpr (std::same_as<T, create_virtual_mic>)
                            {
                                auto node = core->create<pw::node>(pw::factory{
                                    .name = "adapter",
                                    .props =
                                        {
                                            {"node.name", msg.name},
                                            {"media.class", "Audio/Source/Virtual"},
                                            {"factory.name", "support.null-audio-sink"},
                                        },
                                });

                                created.emplace_back(std::move(node.get()));
                            }
                            else if constexpr (std::same_as<T, check_node>)
                            {
                                auto &mic = created.back();

                                if (!mic.has_value())
                                {
                                    std::cerr << "Failed to create node!" << std::endl;

                                    auto error = mic.error();
                                    std::cerr << error.message << std::endl;

                                    return;
                                }

                                std::cout << "Created successfully!" << std::endl;
                                std::cout << mic.value().id() << std::endl;
                            }
                            else if constexpr (std::same_as<T, terminate>)
                            {
                                exit = true;
                                loop->quit();
                            }
                        });

        while (!exit)
        {
            loop->run();
        }
    };

    std::thread t1{thread, std::move(receiver)};

    sender.send(create_virtual_mic{"Test Node"});
    std::cin.get();
    sender.send(check_node{});
    std::cin.get();
    sender.send(terminate{});

    t1.join();

    return 0;
}