#pragma once

#include "info.hpp"
#include "../proxy/proxy.hpp"

struct pw_link;

namespace pipewire
{
    class link_listener;

    struct link_factory
    {
        std::uint32_t input;
        std::uint32_t output;
    };

    class link final : public proxy
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~link() final;

      public:
        link(link &&) noexcept;
        link(proxy &&, link_info);

      public:
        link &operator=(link &&) noexcept;

      public:
        [[nodiscard]] pw_link *get() const;
        [[nodiscard]] link_info info() const;

      public:
        template <class Listener = link_listener>
        [[rk::needs_update]] [[nodiscard]] Listener listen() = delete;

      public:
        [[nodiscard]] operator pw_link *() const &;
        [[nodiscard]] operator pw_link *() const && = delete;

      public:
        [[rk::needs_update]] static lazy<expected<link>> bind(pw_link *);

      public:
        static const char *type;
        static const std::uint32_t version;
    };

    template <>
    link_listener link::listen();
} // namespace pipewire
