#pragma once
#include "info.hpp"
#include "../proxy.hpp"

#include "../utils/annotations.hpp"
struct pw_link;
namespace pipewire
{
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
        static [[needs_update]] lazy_expected<link> bind(pw_link *);

      public:
        [[nodiscard]] link_info info() const;

      public:
        [[nodiscard]] pw_link *get() const;

      public:
        static const std::string type;
        static const std::uint32_t version;
    };
} // namespace pipewire
#include "../utils/annotations.hpp"