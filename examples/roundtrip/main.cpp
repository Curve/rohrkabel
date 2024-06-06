#include <format>
#include <iostream>

#include <rohrkabel/node/node.hpp>

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

    auto on_global = [&](const pw::global &global) {
        if (global.type != pw::node::type)
        {
            return;
        }

        auto node = reg->bind<pw::node>(global.id).get();

        if (!node.has_value())
        {
            std::cout << std::format("failed to bind {}: {}", global.id, node.error().message) << std::endl;
            return;
        }

        auto info = node->info();

        std::cout << std::format("Node ({}): ", info.id) << std::endl;
        static auto indent = std::string{4, ' '};

        for (const auto &prop : info.props)
        {
            std::cout << std::format("{} \"{}\": {}", indent, prop.first, prop.second) << std::endl;
        }
    };

    listener.on<pw::registry_event::global>(on_global);
    core->update();

    return 0;
}
