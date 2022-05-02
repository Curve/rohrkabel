#pragma once
#include "info.hpp"
#include "global.hpp"
#include "listener.hpp"

#include <memory>
#include <cstdint>

struct pw_node;
namespace pipewire
{
    class registry;
    class node : listener
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~node();

      public:
        node(node &&) noexcept;
        node(registry &, const global &);

      public:
        [[nodiscard]] node_info info() const;

      public:
        [[nodiscard]] pw_node *get() const;
    };
} // namespace pipewire