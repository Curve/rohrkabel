#pragma once
#include "info.hpp"
#include "../proxy.hpp"
#include "../spa/pod/pod.hpp"

#include "../utils/annotations.hpp"
struct pw_device;
namespace pipewire
{
    class device final : public proxy
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~device() final;

      public:
        device(device &&) noexcept;
        device(proxy &&, device_info);

      public:
        device &operator=(device &&) noexcept;

      public:
        static [[needs_update]] lazy_expected<device> bind(pw_device *);

      public:
        [[needs_update]] void set_param(std::uint32_t id, const spa::pod &pod);

      public:
        [[nodiscard]] device_info info() const;
        [[nodiscard]] [[needs_update]] std::future<std::map<std::uint32_t, spa::pod>> params() const;

      public:
        [[nodiscard]] pw_device *get() const;

      public:
        static const std::string type;
        static const std::uint32_t version;
    };
} // namespace pipewire
#include "../utils/annotations.hpp"