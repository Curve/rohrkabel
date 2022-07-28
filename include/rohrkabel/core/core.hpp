#pragma once
#include "events.hpp"
#include "../context.hpp"

#include "../proxy.hpp"
#include "../node/node.hpp"
#include "../link/link.hpp"

#include <memory>

#include "../utils/annotations.hpp"
struct pw_core;
namespace pipewire
{
    enum class update_strategy
    {
        sync,
        none,
    };

    class core
    {
        struct impl;
        template <typename Source, typename Desired> using tie_to_t = std::enable_if_t<std::is_same_v<Source, Desired>, Desired>;

      private:
        context &m_context;
        std::unique_ptr<impl> m_impl;

      private:
        template <update_strategy Strategy> void update();

      public:
        ~core();
        core(context &);

      public:
        void update(update_strategy strategy = update_strategy::sync);

      public:
        [[nodiscard]] int sync(int seq);

      public:
        template <class EventListener> [[needs_update]] [[nodiscard]] EventListener listen() = delete;

        template <typename T = proxy>
        [[nodiscard]] [[needs_update]] lazy_expected<T> create(const std::string &factory_name, const properties &props, const std::string &type, std::uint32_t version,
                                                               update_strategy strategy = update_strategy::sync) = delete;

      public:
        template <typename Type>
        [[nodiscard]] [[needs_update]] lazy_expected<tie_to_t<Type, link>> create_simple(std::uint32_t input_port, std::uint32_t output_port,
                                                                                         update_strategy strategy = update_strategy::sync) = delete;

      public:
        [[nodiscard]] pw_core *get() const;
        [[nodiscard]] context &get_context();
    };

    template <> void core::update<update_strategy::none>();
    template <> void core::update<update_strategy::sync>();

    template <> core_listener core::listen();

    template <> lazy_expected<node> core::create(const std::string &, const properties &, const std::string &, std::uint32_t, update_strategy);
    template <> lazy_expected<link> core::create(const std::string &, const properties &, const std::string &, std::uint32_t, update_strategy);
    template <> lazy_expected<proxy> core::create(const std::string &, const properties &, const std::string &, std::uint32_t, update_strategy);

    template <> lazy_expected<link> core::create_simple<link>(std::uint32_t input, std::uint32_t output, update_strategy strategy);
} // namespace pipewire
#include "../utils/annotations.hpp"