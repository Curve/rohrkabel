#pragma once
#include "listener.hpp"
#include "properties.hpp"

#include <memory>
#include <string>
#include <cstdint>

struct pw_proxy;
namespace pipewire
{
    class core;
    class proxy : listener
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
        [[nodiscard]] std::uint32_t id() const;

      public:
        [[nodiscard]] pw_proxy *get() const;
    };
} // namespace pipewire