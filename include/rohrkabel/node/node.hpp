#pragma once

#include "info.hpp"

#include "../properties.hpp"
#include "../proxy/proxy.hpp"
#include "../spa/pod/pod.hpp"

#include <map>
#include <set>

#include <memory>
#include <cstdint>

struct pw_node;

namespace pipewire
{
    class node;
    class node_listener;

    struct null_factory
    {
        using result = node;

      public:
        enum class kind : std::uint8_t
        {
            sink,
            source,
        };

      public:
        kind type;
        std::string name;
        std::set<std::string> positions;

      public:
        properties props{properties::create()};
    };

    class node final : public proxy
    {
        struct impl;

      public:
        using raw_type = pw_node;

      private:
        using params_t = std::map<std::uint32_t, spa::pod>;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        node(proxy &&, node_info);

      public:
        node(node &&) noexcept;
        node &operator=(node &&) noexcept;

      public:
        ~node() final;

      public:
        void set_param(std::uint32_t id, std::uint32_t flags, const spa::pod &pod);

      public:
        [[nodiscard]] node_info info() const;
        [[nodiscard]] [[rk::needs_sync]] lazy<params_t> params() const;

      public:
        template <detail::listener<raw_type> Listener = node_listener>
        [[nodiscard]] Listener listen() const;

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] raw_type *release() &&;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        static task<node> bind(raw_type *);

      public:
        static const char *type;
        static const std::uint32_t version;
    };
} // namespace pipewire

#include "node.inl"
