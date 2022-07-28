#pragma once
#include "info.hpp"
#include "../proxy.hpp"

#include "../utils/annotations.hpp"
struct pw_client;
namespace pipewire
{
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
        static [[needs_update]] lazy_expected<client> bind(pw_client *);

      public:
        client &operator=(client &&) noexcept;

      public:
        [[nodiscard]] client_info info() const;

      public:
        [[nodiscard]] pw_client *get() const;

      public:
        static const std::string type;
        static const std::uint32_t version;
    };
} // namespace pipewire
#include "../utils/annotations.hpp"