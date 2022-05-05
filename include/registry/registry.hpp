#pragma once
#include "events.hpp"
#include "core/core.hpp"

#include "node/node.hpp"
#include "port/port.hpp"
#include "metadata/metadata.hpp"

#include <memory>

#include "annotations.hpp"
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
        template <class EventListener> [[needs_sync]] [[nodiscard]] EventListener listen() = delete;
        template <class Interface> [[needs_sync]] [[nodiscard]] Interface bind(const global &, bool auto_sync = true) = delete;

      public:
        [[nodiscard]] core &get_core();
        [[nodiscard]] pw_registry *get() const;
    };

    template <> registry_listener registry::listen<registry_listener>();

    template <> node registry::bind<node>(const global &, bool);
    template <> port registry::bind<port>(const global &, bool);
    template <> metadata registry::bind<metadata>(const global &, bool);
} // namespace pipewire
#include "annotations.hpp"