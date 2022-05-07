#pragma once
#include "info.hpp"
#include "global.hpp"

#include <memory>
#include <cstdint>

struct pw_device;
namespace pipewire
{
    class registry;
    class device
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~device();

      public:
        device(device &&) noexcept;
        device(registry &, const global &);

      public:
        device &operator=(device &&) noexcept;

      public:
        [[nodiscard]] device_info info() const;

      public:
        [[nodiscard]] pw_device *get() const;

      public:
        static const std::string type;
        static const std::uint32_t version;
    };
} // namespace pipewire