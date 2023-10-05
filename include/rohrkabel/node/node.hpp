#pragma once
#include "info.hpp"
#include "../proxy.hpp"
#include "../utils/lazy.hpp"
#include "../spa/pod/pod.hpp"

#include <map>
#include <memory>
#include <cstdint>

struct pw_node;

namespace pipewire
{
    class node final : public proxy
    {
        struct impl;

      private:
        using params_t = std::map<std::uint32_t, spa::pod>;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~node() final;

      public:
        node(node &&) noexcept;
        node(proxy &&, node_info);

      public:
        node &operator=(node &&) noexcept;

      public:
        [[rk::needs_update]] void set_param(std::uint32_t id, std::uint32_t flags, const spa::pod &pod);

      public:
        [[nodiscard]] [[rk::needs_update]] lazy<params_t> params();

      public:
        [[nodiscard]] pw_node *get() const;
        [[nodiscard]] node_info info() const;

      public:
        [[nodiscard]] operator pw_node *() const &;
        [[nodiscard]] operator pw_node *() const && = delete;

      public:
        [[rk::needs_update]] static lazy<expected<node>> bind(pw_node *);

      public:
        static const char *type;
        static const std::uint32_t version;
    };
} // namespace pipewire