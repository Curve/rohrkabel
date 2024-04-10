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

    class registry_listener : public listener<registry_event,                                                      //
                                              ereignis::event<registry_event::global, void(const global &)>,       //
                                              ereignis::event<registry_event::global_removed, void(std::uint32_t)> //
                                              >
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~registry_listener();

      public:
        registry_listener(pw_registry *);
        registry_listener(registry_listener &&) noexcept;
    };
} // namespace pipewire
