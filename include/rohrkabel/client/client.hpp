#pragma once
#include "../proxy.hpp"
#include "../spa/dict.hpp"
#include "../utils/lazy.hpp"

struct pw_client;

namespace pipewire
{
    struct client_info
    {
        spa::dict props;
        std::uint32_t id;
        std::uint64_t change_mask;
    };

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
        [[nodiscard]] operator pw_client *() const &;
        [[nodiscard]] operator pw_client *() const && = delete;

      public:
        [[rk::needs_update]] static lazy<expected<client>> bind(pw_client *);

      public:
        static const char *type;
        static const std::uint32_t version;
    };
} // namespace pipewire