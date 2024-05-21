#pragma once

#include "info.hpp"
#include "../proxy/proxy.hpp"
#include "../spa/pod/pod.hpp"

#include <map>
#include <memory>
#include <cstdint>

struct pw_port;

namespace pipewire
{
    class port_listener;

    class port final : public proxy
    {
        struct impl;

      public:
        using raw_type = pw_port;

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
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] port_info info() const;

      public:
        template <class Listener = port_listener>
            requires valid_listener<Listener, raw_type>
        [[rk::needs_update]] [[nodiscard]] Listener listen();

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[rk::needs_update]] static lazy<expected<port>> bind(raw_type *);

      public:
        static const char *type;
        static const std::uint32_t version;
    };
} // namespace pipewire

#include "port.inl"
