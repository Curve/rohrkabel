#pragma once

#include "property.hpp"
#include "../proxy/proxy.hpp"

#include <map>
#include <string>

struct pw_metadata;

namespace pipewire
{
    class metadata_listener;

    class metadata final : public proxy
    {
        struct impl;

      public:
        using raw_type = pw_metadata;

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
        [[rk::needs_update]] void clear_property(std::uint32_t id, const std::string &key);
        [[rk::needs_update]] void set_property(std::uint32_t id, std::string key, std::string type, std::string value);

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] properties_t properties() const;

      public:
        template <class Listener = metadata_listener>
            requires valid_listener<Listener, raw_type>
        [[rk::needs_update]] [[nodiscard]] Listener listen();

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[rk::needs_update]] static lazy<expected<metadata>> bind(raw_type *);

      public:
        static const char *type;
        static const std::uint32_t version;
    };
} // namespace pipewire
