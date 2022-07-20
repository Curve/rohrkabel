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
        template <class EventListener> [[needs_update]] [[nodiscard]] EventListener listen() = delete;
        template <class Interface> [[needs_update]] [[nodiscard]] Interface bind(std::uint32_t id, update_strategy strategy = update_strategy::internal) = delete;

      public:
        [[nodiscard]] core &get_core();
        [[nodiscard]] pw_registry *get() const;
    };

    template <> registry_listener registry::listen();

    template <> node registry::bind(std::uint32_t, update_strategy);
    template <> port registry::bind(std::uint32_t, update_strategy);
    template <> client registry::bind(std::uint32_t, update_strategy);
    template <> device registry::bind(std::uint32_t, update_strategy);
    template <> metadata registry::bind(std::uint32_t, update_strategy);
} // namespace pipewire
#include "../utils/annotations.hpp"