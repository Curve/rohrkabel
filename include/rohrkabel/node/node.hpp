#pragma once
#include "info.hpp"
#include "../proxy.hpp"
#include "../spa/pod/pod.hpp"

#include "../utils/annotations.hpp"
struct pw_node;
namespace pipewire
{
    class node final : public proxy
    {
        struct impl;

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
        static [[needs_update]] lazy_expected<node> bind(pw_node *);

      public:
        [[needs_update]] void set_param(std::uint32_t id, const spa::pod &pod);

      public:
        [[nodiscard]] node_info info() const;
        [[nodiscard]] [[needs_update]] std::future<std::map<std::uint32_t, spa::pod>> params();

      public:
        [[nodiscard]] pw_node *get() const;

      public:
        static const std::string type;
        static const std::uint32_t version;
    };
} // namespace pipewire
#include "../utils/annotations.hpp"