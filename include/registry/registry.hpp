#pragma once
#include "events.hpp"
#include "core/core.hpp"

#include "node/node.hpp"
#include "port/port.hpp"
#include "metadata/metadata.hpp"

#include <memory>

struct pw_registry;
namespace pipewire
{
    class registry
    {
        struct impl;

      private:
        core &m_core;
        std::unique_ptr<impl> m_impl;

      public:
        ~registry();
        registry(core &);

      public:
        template <class EventListener> [[nodiscard]] EventListener listen() = delete;
        template <class Interface> [[nodiscard]] Interface bind(const global &) = delete;

      public:
        [[nodiscard]] core &get_core();
        [[nodiscard]] pw_registry *get() const;
    };

    template <> registry_listener registry::listen<registry_listener>();

    template <> node registry::bind<node>(const global &);
    template <> port registry::bind<port>(const global &);
    template <> metadata registry::bind<metadata>(const global &);
} // namespace pipewire