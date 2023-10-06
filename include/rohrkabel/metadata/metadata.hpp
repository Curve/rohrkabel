#pragma once
#include "property.hpp"
#include "../proxy.hpp"

#include <map>

struct pw_metadata;

namespace pipewire
{
    class metadata final : public proxy
    {
        struct impl;

      private:
        using properties_t = std::map<std::string, metadata_property>;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~metadata() final;

      public:
        metadata(metadata &&) noexcept;
        metadata(proxy &&, properties_t);

      public:
        metadata &operator=(metadata &&) noexcept;

      public:
        [[nodiscard]] pw_metadata *get() const;
        [[nodiscard]] properties_t properties() const;

      public:
        [[nodiscard]] operator pw_metadata *() const &;
        [[nodiscard]] operator pw_metadata *() const && = delete;

      public:
        [[rk::needs_update]] static lazy<expected<metadata>> bind(pw_metadata *);

      public:
        static const char *type;
        static const std::uint32_t version;
    };
} // namespace pipewire