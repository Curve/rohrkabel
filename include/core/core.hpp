#pragma once
#include "proxy.hpp"
#include "events.hpp"
#include "context.hpp"

#include <memory>

struct pw_core;
namespace pipewire
{
    class core
    {
        struct impl;

      private:
        context &m_context;
        std::unique_ptr<impl> m_impl;

      public:
        ~core();
        core(context &);

      public:
        void sync();
        [[nodiscard]] int sync(int seq);

      public:
        template <class EventListener> [[nodiscard]] EventListener listen() = delete;
        [[nodiscard]] proxy create(const std::string &factory_name, const properties &properties, const std::string &type, std::uint32_t version);

      public:
        [[nodiscard]] pw_core *get() const;
        [[nodiscard]] context &get_context();
    };

    template <> core_listener core::listen<core_listener>();
} // namespace pipewire