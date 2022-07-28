#pragma once
#include "../proxy.hpp"
#include "property.hpp"

#include "../utils/annotations.hpp"
struct pw_metadata;
namespace pipewire
{
    class metadata final : public proxy
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~metadata() final;

      public:
        metadata(metadata &&) noexcept;
        metadata(proxy &&, std::map<const std::string, const metadata_property>);

      public:
        metadata &operator=(metadata &&) noexcept;

      public:
        static [[needs_update]] lazy_expected<metadata> bind(pw_metadata *);

      public:
        [[nodiscard]] std::map<const std::string, const metadata_property> properties() const;

      public:
        [[nodiscard]] pw_metadata *get() const;

      public:
        static const std::string type;
        static const std::uint32_t version;
    };
} // namespace pipewire
#include "../utils/annotations.hpp"