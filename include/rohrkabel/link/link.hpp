#pragma once

#include "info.hpp"

#include "../proxy/proxy.hpp"

struct pw_link;

namespace pipewire
{
    class link;
    class link_listener;

    struct link_factory
    {
        using result = link;

      public:
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
        link(proxy &&, link_info);

      public:
        link(link &&) noexcept;
        link &operator=(link &&) noexcept;

      public:
        ~link() final;

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] link_info info() const;

      public:
        template <detail::Listener<raw_type> Listener = link_listener>
        [[nodiscard]] Listener listen() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        static task<link> bind(raw_type *);

      public:
        static const char *type;
        static const std::uint32_t version;
    };
} // namespace pipewire

#include "link.inl"
