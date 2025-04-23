#pragma once

#include "registry.hpp"

#include "../global.hpp"
#include "../utils/listener.hpp"

#include <memory>

namespace pipewire
{
    enum class registry_event : std::uint8_t
    {
        global,
        global_removed
    };

    class registry_listener : public listener<                                                         //
                                  ereignis::event<registry_event::global, void(global)>,               //
                                  ereignis::event<registry_event::global_removed, void(std::uint32_t)> //
                                  >
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        registry_listener(registry::raw_type *);

      public:
        ~registry_listener();

      public:
        static const std::uint32_t version;
    };
} // namespace pipewire
