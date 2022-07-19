#pragma once
#include "info.hpp"
#include "../proxy.hpp"
#include "../global.hpp"
#include "../spa/pod/pod.hpp"

#include <map>
#include <future>
#include <memory>
#include <cstdint>

#include "../utils/annotations.hpp"
struct pw_device;
namespace pipewire
{
    class registry;
    class device final : public proxy
    {
        friend class registry;
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~device() final;

      public:
        device(pw_device *);
        device(device &&) noexcept;
        device(registry &, std::uint32_t);

      public:
        device &operator=(device &&) noexcept;

      public:
        [[needs_sync]] void set_param(std::uint32_t id, const spa::pod &pod);

      public:
        [[nodiscard]] device_info info() const;
        [[nodiscard]] [[needs_sync]] std::future<std::map<std::uint32_t, spa::pod>> params() const;

      public:
        [[nodiscard]] pw_device *get() const;

      public:
        static const std::string type;
        static const std::uint32_t version;
    };
} // namespace pipewire
#include "../utils/annotations.hpp"