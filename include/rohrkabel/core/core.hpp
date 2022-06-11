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
    class core
    {
        struct impl;

        using factories_t = factory_handler<                    //
            factory<link_factory, std::uint32_t, std::uint32_t> //
            >;

      private:
        context &m_context;
        std::unique_ptr<impl> m_impl;

      public:
        ~core();
        core(context &);

      public:
        void sync();
        void sync(std::size_t amount);
        [[nodiscard]] int sync(int seq);

      public:
        template <class EventListener> [[needs_sync]] [[nodiscard]] EventListener listen() = delete;
        [[nodiscard]] [[needs_sync]] proxy create(const std::string &factory_name, const properties &props, const std::string &type, std::uint32_t version, bool auto_sync = true);

      public:
        template <typename Type> [[nodiscard]] [[needs_sync]] Type create(const factories_t::get_t<Type> &params, bool auto_sync = true) = delete;

      public:
        [[nodiscard]] pw_core *get() const;
        [[nodiscard]] context &get_context();
    };

    template <> core_listener core::listen<core_listener>();
    template <> link_factory core::create<link_factory>(const factories_t::get_t<link_factory> &, bool);
} // namespace pipewire
#include "../utils/annotations.hpp"