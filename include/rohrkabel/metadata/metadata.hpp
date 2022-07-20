#pragma once
#include "property.hpp"

#include <map>
#include <memory>

struct pw_metadata;
namespace pipewire
{
    class registry;
    class metadata
    {
        struct impl;
        using properties_t = std::map<const std::string, const metadata_property>;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~metadata();

      public:
        metadata(metadata &&) noexcept;
        metadata(registry &, std::uint32_t);

      public:
        metadata &operator=(metadata &&) noexcept;

      public:
        [[nodiscard]] properties_t properties() const;

      public:
        [[nodiscard]] pw_metadata *get() const;

      public:
        static const std::string type;
        static const std::uint32_t version;
    };
} // namespace pipewire