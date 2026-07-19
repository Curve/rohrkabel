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
        metadata(proxy &&, properties_t);

      public:
        metadata(metadata &&) noexcept;
        metadata &operator=(metadata &&) noexcept;

      public:
        ~metadata() final;

      public:
        void clear_property(std::uint32_t id, const std::string &key);
        void set_property(std::uint32_t id, std::string key, std::string type, std::string value);

      public:
        [[nodiscard]] properties_t properties() const;

      public:
        template <detail::listener<raw_type> Listener = metadata_listener>
        [[nodiscard]] Listener listen() const;

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] raw_type *release() &&;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[rk::needs_sync]] static task<metadata> bind(raw_type *);

      public:
        static const char *type;
        static const std::uint32_t version;
    };
} // namespace pipewire

#include "metadata.inl"
