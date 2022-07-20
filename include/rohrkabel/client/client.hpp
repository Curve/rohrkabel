#pragma once
#include "info.hpp"

#include <memory>

struct pw_client;
namespace pipewire
{
    class registry;
    class client
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~client();

      public:
        client(client &&) noexcept;
        client(registry &, std::uint32_t);

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