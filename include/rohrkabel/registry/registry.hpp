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
        template <class T> [[needs_update]] [[nodiscard]] lazy_expected<T> bind(std::uint32_t id, update_strategy strategy = update_strategy::sync) = delete;

      public:
        [[nodiscard]] core &get_core();
        [[nodiscard]] pw_registry *get() const;
    };

    template <> registry_listener registry::listen();

    template <> lazy_expected<node> registry::bind(std::uint32_t, update_strategy);
    template <> lazy_expected<port> registry::bind(std::uint32_t, update_strategy);
    template <> lazy_expected<client> registry::bind(std::uint32_t, update_strategy);
    template <> lazy_expected<device> registry::bind(std::uint32_t, update_strategy);
    template <> lazy_expected<metadata> registry::bind(std::uint32_t, update_strategy);
} // namespace pipewire
#include "../utils/annotations.hpp"