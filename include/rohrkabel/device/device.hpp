#pragma once

#include "info.hpp"
#include "../proxy/proxy.hpp"
#include "../spa/pod/pod.hpp"

#include <map>
#include <memory>
#include <cstdint>

struct pw_device;

namespace pipewire
{
    class device_listener;

    class device final : public proxy
    {
        struct impl;

      private:
        using underlying = std::map<std::uint32_t, spa::pod>;

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
        [[rk::needs_update]] void set_param(std::uint32_t id, std::uint32_t flags, const spa::pod &pod);

      public:
        [[nodiscard]] [[rk::needs_update]] lazy<underlying> params() const;

      public:
        template <class Listener = device_listener>
        [[rk::needs_update]] [[nodiscard]] Listener listen() = delete;

      public:
        [[nodiscard]] pw_device *get() const;
        [[nodiscard]] device_info info() const;

      public:
        [[nodiscard]] operator pw_device *() const &;
        [[nodiscard]] operator pw_device *() const && = delete;

      public:
        [[rk::needs_update]] static lazy<expected<device>> bind(pw_device *);

      public:
        static const char *type;
        static const std::uint32_t version;
    };

    template <>
    device_listener device::listen();
} // namespace pipewire
