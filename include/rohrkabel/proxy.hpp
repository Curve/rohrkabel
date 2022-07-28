#pragma once
#include "error.hpp"
#include "properties.hpp"

#include <memory>
#include <string>
#include <cstdint>

#include "utils/annotations.hpp"
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
        proxy(pw_proxy *);
        proxy(proxy &&) noexcept;

      public:
        proxy &operator=(proxy &&) noexcept;

      public:
        static [[needs_update]] lazy_expected<proxy> bind(pw_proxy *);

      public:
        [[nodiscard]] std::uint32_t id() const;
        [[nodiscard]] std::string type() const;
        [[nodiscard]] std::uint32_t version() const;

      public:
        [[nodiscard]] pw_proxy *get() const;
    };
} // namespace pipewire
#include "utils/annotations.hpp"