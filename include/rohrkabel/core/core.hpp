#pragma once
#include "events.hpp"
#include "../proxy.hpp"
#include "../context.hpp"
#include "../link_factory.hpp"
#include "../utils/factories/factories.hpp"

#include <memory>

#include "../utils/annotations.hpp"
struct pw_core;
namespace pipewire
{
    enum class update_strategy
    {
        wait_lock,
        internal,
        sync,
        best,
        none,
    };

    class core
    {
        struct impl;

        using factories_t = factory_handler<                    //
            factory<link_factory, std::uint32_t, std::uint32_t> //
            >;

      private:
        context &m_context;
        std::unique_ptr<impl> m_impl;

      private:
        template <update_strategy Strategy> void update();

      public:
        ~core();
        core(context &);

      public:
        void update(update_strategy strategy = update_strategy::best);

      public:
        [[nodiscard]] int sync(int seq);

      public:
        template <class EventListener> [[needs_update]] [[nodiscard]] EventListener listen() = delete;
        [[nodiscard]] [[needs_update]] proxy create(const std::string &factory_name, const properties &props, const std::string &type, std::uint32_t version,
                                                    update_strategy strategy = update_strategy::internal);

      public:
        template <typename Type> [[nodiscard]] [[needs_update]] Type create(const factories_t::get_t<Type> &param, update_strategy strategy = update_strategy::internal) = delete;

      public:
        [[nodiscard]] pw_core *get() const;
        [[nodiscard]] context &get_context();
    };

    template <> void core::update<update_strategy::none>();
    template <> void core::update<update_strategy::best>();
    template <> void core::update<update_strategy::sync>();
    template <> void core::update<update_strategy::internal>();
    template <> [[thread_safe]] void core::update<update_strategy::wait_lock>();

    template <> core_listener core::listen();
    template <> link_factory core::create(const factories_t::get_t<link_factory> &, update_strategy);
} // namespace pipewire
#include "../utils/annotations.hpp"