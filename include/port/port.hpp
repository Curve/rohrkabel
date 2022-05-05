#pragma once
#include "info.hpp"
#include "global.hpp"

#include <memory>

struct pw_port;
namespace pipewire
{
    class registry;
    class port
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~port();

      public:
        port(port &&) noexcept;
        port(registry &, const global &);

      public:
        port &operator=(port &&) noexcept;

      public:
        [[nodiscard]] port_info info() const;

      public:
        [[nodiscard]] pw_port *get() const;
    };
} // namespace pipewire