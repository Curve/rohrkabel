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
        port(proxy &&, port_info);

      public:
        port(port &&) noexcept;
        port &operator=(port &&) noexcept;

      public:
        ~port() final;

      public:
        [[nodiscard]] port_info info() const;
        [[nodiscard]] [[rk::needs_sync]] lazy<params_t> params() const;

      public:
        template <detail::listener<raw_type> Listener = port_listener>
        [[nodiscard]] Listener listen() const;

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] raw_type *release() &&;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        static task<port> bind(raw_type *);

      public:
        static const char *type;
        static const std::uint32_t version;
    };
} // namespace pipewire

#include "port.inl"
