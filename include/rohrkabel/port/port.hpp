#pragma once

#include "info.hpp"
#include "../proxy.hpp"
#include "../spa/pod/pod.hpp"

#include <map>
#include <memory>
#include <cstdint>

struct pw_port;

namespace pipewire
{
    class port final : public proxy
    {
        struct impl;

      private:
        using params_t = std::map<std::uint32_t, spa::pod>;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~port() final;

      public:
        port(port &&) noexcept;
        port(proxy &&, port_info);

      public:
        port &operator=(port &&) noexcept;

      public:
        [[nodiscard]] [[rk::needs_update]] lazy<params_t> params() const;

      public:
        [[nodiscard]] pw_port *get() const;
        [[nodiscard]] port_info info() const;

      public:
        [[nodiscard]] operator pw_port *() const &;
        [[nodiscard]] operator pw_port *() const && = delete;

      public:
        [[rk::needs_update]] static lazy<expected<port>> bind(pw_port *);

      public:
        static const char *type;
        static const std::uint32_t version;
    };
} // namespace pipewire
