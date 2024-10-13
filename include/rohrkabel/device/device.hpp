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

      public:
        using raw_type = pw_device;

      private:
        using params_t = std::map<std::uint32_t, spa::pod>;

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
        [[nodiscard]] [[rk::needs_update]] lazy<params_t> params() const;

      public:
        template <class Listener = device_listener>
            requires detail::valid_listener<Listener, raw_type>
        [[rk::needs_update]] [[nodiscard]] Listener listen();

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] device_info info() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[rk::needs_update]] static lazy<expected<device>> bind(raw_type *);

      public:
        static const char *type;
        static const std::uint32_t version;
    };
} // namespace pipewire

#include "device.inl"
