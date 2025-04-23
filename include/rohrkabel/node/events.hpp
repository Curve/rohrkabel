#pragma once

#include "info.hpp"
#include "node.hpp"

#include "../utils/listener.hpp"

#include <memory>

namespace pipewire
{
    enum class node_event : std::uint8_t
    {
        info,
        param,
    };

    class node_listener
        : public listener<
              ereignis::event<node_event::info, void(node_info)>,                                                          //
              ereignis::event<node_event::param, void(int, std::uint32_t, std::uint32_t, std::uint32_t, const spa::pod &)> //
              >
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        node_listener(node::raw_type *);

      public:
        ~node_listener();

      public:
        static const std::uint32_t version;
    };
} // namespace pipewire
