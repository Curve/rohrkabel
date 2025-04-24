#include <print>
#include <iostream>

#include <thread>
#include <optional>

#include <rohrkabel/node/node.hpp>
#include <rohrkabel/registry/registry.hpp>

#include <rohrkabel/channel/channel.hpp>

namespace pw = pipewire;

struct create_virtual_mic
{
    std::string name;
};

struct terminate
{
};

using recipe = pw::recipe<create_virtual_mic, terminate>;

template <typename... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};

int main()
{
    auto thread = [](recipe::receiver receiver)
    {
        auto loop    = pw::main_loop::create();
        auto context = pw::context::create(loop);
        auto core    = pw::core::create(context);
        auto reg     = pw::registry::create(core);

        auto created = std::optional<pw::node>{};
        auto stopped = std::atomic<bool>{false};

        auto visitor = overload{[&](create_virtual_mic msg) -> coco::stray
                                {
                                    // We check if the stopped flag is set, so that we don't await something after the run-loop
                                    // was stopped.

                                    if (stopped)
                                    {
                                        co_return;
                                    }

                                    auto node = co_await core->create(pw::null_sink_factory{
                                        .name      = msg.name,
                                        .positions = {"FL", "FR"},
                                    });

                                    if (!node)
                                    {
                                        std::println(stderr, "Failed to create node: {}", node.error().message);
                                        co_return;
                                    }

                                    std::println("Created node: {}", node->id());
                                    created.emplace(std::move(node.value()));
                                },
                                [&](terminate) -> coco::stray
                                {
                                    stopped = true;
                                    loop->quit();
                                    co_return;
                                }};

        receiver.attach(loop, visitor);
        loop->run();
    };

    auto [sender, receiver] = pw::channel<recipe>();
    std::jthread t{thread, std::move(receiver)};

    auto name = std::string{};

    std::print("Name of node: ");
    std::getline(std::cin >> std::ws, name);

    sender.send(create_virtual_mic{std::move(name)});

    std::cin.get();
    sender.send(terminate{});

    return 0;
}
