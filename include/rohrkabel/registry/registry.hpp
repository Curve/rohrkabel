#pragma once
#include "events.hpp"
#include "../core/core.hpp"

#include "../node/node.hpp"
#include "../port/port.hpp"
#include "../client/client.hpp"
#include "../device/device.hpp"
#include "../metadata/metadata.hpp"

#include <memory>

#include "../utils/annotations.hpp"
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
        template <class Interface> [[needs_sync]] [[nodiscard]] Interface bind(std::uint32_t, bool auto_sync = true) = delete;

      public:
        [[nodiscard]] core &get_core();
        [[nodiscard]] pw_registry *get() const;
    };

    template <> registry_listener registry::listen<registry_listener>();

    template <> node registry::bind<node>(std::uint32_t, bool);
    template <> port registry::bind<port>(std::uint32_t, bool);
    template <> client registry::bind<client>(std::uint32_t, bool);
    template <> device registry::bind<device>(std::uint32_t, bool);
    template <> metadata registry::bind<metadata>(std::uint32_t, bool);
} // namespace pipewire
#include "../utils/annotations.hpp"