#pragma once
#include "info.hpp"
#include "global.hpp"
#include "spa/pod/pod.hpp"

#include <map>
#include <memory>

#include "utils/annotations.hpp"
struct pw_port;
namespace pipewire
{
    class registry;
    class port
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~port();

      public:
        port(port &&) noexcept;
        port(registry &, std::uint32_t);

      public:
        port &operator=(port &&) noexcept;

      public:
        [[nodiscard]] port_info info() const;
        [[nodiscard]] [[needs_sync]] const std::map<std::uint32_t, spa::pod> &params() const;

      public:
        [[nodiscard]] pw_port *get() const;

      public:
        static const std::string type;
        static const std::uint32_t version;
    };
} // namespace pipewire
#include "utils/annotations.hpp"