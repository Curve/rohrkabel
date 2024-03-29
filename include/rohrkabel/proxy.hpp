#pragma once

#include "spa/dict.hpp"
#include "utils/lazy.hpp"

#include <string>
#include <memory>
#include <cstdint>

struct pw_proxy;

namespace pipewire
{
    class proxy
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        virtual ~proxy();

      public:
        proxy(proxy &&) noexcept;
        proxy(pw_proxy *, spa::dict);

      public:
        proxy &operator=(proxy &&) noexcept;

      public:
        [[nodiscard]] spa::dict props() const;
        [[nodiscard]] std::string type() const;
        [[nodiscard]] std::uint32_t id() const;
        [[nodiscard]] std::uint32_t version() const;

      public:
        [[nodiscard]] pw_proxy *get() const;

      public:
        [[nodiscard]] operator pw_proxy *() const &;
        [[nodiscard]] operator pw_proxy *() const && = delete;

      public:
        [[rk::needs_update]] static lazy<expected<proxy>> bind(pw_proxy *);
    };
} // namespace pipewire
