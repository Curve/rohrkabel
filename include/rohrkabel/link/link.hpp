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

      public:
        using raw_type = pw_link;

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
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] link_info info() const;

      public:
        template <class Listener = link_listener>
            requires valid_listener<Listener, raw_type>
        [[rk::needs_update]] [[nodiscard]] Listener listen();

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[rk::needs_update]] static lazy<expected<link>> bind(raw_type *);

      public:
        static const char *type;
        static const std::uint32_t version;
    };
} // namespace pipewire

#include "link.inl"
