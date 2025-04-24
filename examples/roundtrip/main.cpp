#include <print>

#include <rohrkabel/registry/events.hpp>
#include <rohrkabel/registry/registry.hpp>

namespace pw = pipewire;

// Adapted from: https://docs.pipewire.org/page_tutorial3.html

int main()
{
    auto loop    = pw::main_loop::create();
    auto context = pw::context::create(loop);
    auto core    = pw::core::create(context);
    auto reg     = pw::registry::create(core);

    auto listener = reg->listen();

    auto on_global = [](const pw::global &global)
    {
        std::println("object: id:{} type:{}/{}", global.id, global.type, global.version);
    };

    listener.on<pw::registry_event::global>(on_global);
    core->run_once(); // Or: coco::then(core->sync(), [&](auto) { loop->quit(); }); loop->run();

    return 0;
}
