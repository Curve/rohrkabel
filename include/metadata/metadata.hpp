#pragma once
#include "global.hpp"
#include "listener.hpp"
#include "property.hpp"

#include <memory>

struct pw_metadata;
namespace pipewire
{
    class registry;
    class metadata : listener
    {
        struct impl;
        using properties_t = std::map<const std::string, const metadata_property>;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~metadata();

      public:
        metadata(metadata &&) noexcept;
        metadata(registry &, const global &);

      public:
        [[nodiscard]] properties_t properties() const;

      public:
        [[nodiscard]] pw_metadata *get() const;
    };
} // namespace pipewire