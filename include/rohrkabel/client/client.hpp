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

      public:
        using raw_type = pw_client;

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
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] client_info info() const;

      public:
        template <class Listener = client_listener>
            requires valid_listener<Listener, raw_type>
        [[rk::needs_update]] [[nodiscard]] Listener listen();

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[rk::needs_update]] static lazy<expected<client>> bind(raw_type *);

      public:
        static const char *type;
        static const std::uint32_t version;
    };
} // namespace pipewire

#include "client.inl"
