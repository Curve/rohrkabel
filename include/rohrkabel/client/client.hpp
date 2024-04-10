#pragma once

#include "info.hpp"
#include "../proxy/proxy.hpp"

struct pw_client;

namespace pipewire
{
    class client_listener;

    class client final : public proxy
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~client() final;

      public:
        client(client &&) noexcept;
        client(proxy &&, client_info);

      public:
        client &operator=(client &&) noexcept;

      public:
        [[nodiscard]] pw_client *get() const;
        [[nodiscard]] client_info info() const;

      public:
        template <class Listener = client_listener>
        [[rk::needs_update]] [[nodiscard]] Listener listen() = delete;

      public:
        [[nodiscard]] operator pw_client *() const &;
        [[nodiscard]] operator pw_client *() const && = delete;

      public:
        [[rk::needs_update]] static lazy<expected<client>> bind(pw_client *);

      public:
        static const char *type;
        static const std::uint32_t version;
    };

    template <>
    client_listener client::listen();
} // namespace pipewire
