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
        client(proxy &&, client_info);

      public:
        client(client &&) noexcept;
        client &operator=(client &&) noexcept;

      public:
        ~client() final;

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] client_info info() const;

      public:
        template <detail::Listener<raw_type> Listener = client_listener>
        [[nodiscard]] Listener listen() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        static task<client> bind(raw_type *);

      public:
        static const char *type;
        static const std::uint32_t version;
    };
} // namespace pipewire

#include "client.inl"
