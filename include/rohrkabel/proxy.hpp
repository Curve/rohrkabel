#pragma once
#include "properties.hpp"

#include <memory>
#include <string>
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
        proxy(pw_proxy *);
        proxy(proxy &&) noexcept;

      public:
        proxy &operator=(proxy &&) noexcept;

      public:
        [[nodiscard]] std::uint32_t id() const;

      public:
        [[nodiscard]] pw_proxy *get() const;
        [[nodiscard]] std::uint32_t release();
    };
} // namespace pipewire