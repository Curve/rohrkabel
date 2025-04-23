#pragma once

#include "info.hpp"
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

    struct null_sink_factory
    {
        using result = node;

      public:
        std::string name;
        std::set<std::string> positions;
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
        [[nodiscard]] [[rk::needs_sync]] lazy<params_t> params() const;

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] node_info info() const;

      public:
        template <detail::Listener<raw_type> Listener = node_listener>
        [[nodiscard]] Listener listen() const;

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
