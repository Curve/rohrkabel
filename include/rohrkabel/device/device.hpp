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
        device(proxy &&, device_info);

      public:
        device(device &&) noexcept;
        device &operator=(device &&) noexcept;

      public:
        ~device() final;

      public:
        void set_param(std::uint32_t id, std::uint32_t flags, const spa::pod &pod);

      public:
        [[rk::needs_sync]] [[nodiscard]] lazy<params_t> params() const;

      public:
        template <detail::Listener<raw_type> Listener = device_listener>
        [[nodiscard]] Listener listen() const;

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] device_info info() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        static task<device> bind(raw_type *);

      public:
        static const char *type;
        static const std::uint32_t version;
    };
} // namespace pipewire

#include "device.inl"
