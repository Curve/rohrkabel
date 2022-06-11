#pragma once
#include "properties.hpp"

#include <memory>
#include <string>
#include <cstdint>

struct pw_proxy;
namespace pipewire
{
    class core;
    class proxy
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~proxy();

      public:
        proxy(proxy &&) noexcept;
        proxy(core &, const std::string &, const properties &, const std::string &, std::uint32_t);

      public:
        proxy &operator=(proxy &&) noexcept;

      public:
        [[nodiscard]] std::uint32_t id() const;

      public:
        [[nodiscard]] pw_proxy *get() const;
        [[nodiscard]] std::uint32_t release();
    };
} // namespace pipewire