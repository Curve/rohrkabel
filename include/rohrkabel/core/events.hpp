#pragma once

#include "core.hpp"
#include "info.hpp"

#include "../error.hpp"
#include "../utils/listener.hpp"

#include <memory>

namespace pipewire
{
    enum class core_event : std::uint8_t
    {
        info,
        done,
        error,
    };

    class core_listener : public listener<core_event,                                                            //
                                          ereignis::event<core_event::info, void(const core_info &)>,            //
                                          ereignis::event<core_event::done, void(std::uint32_t, int)>,           //
                                          ereignis::event<core_event::error, void(std::uint32_t, const error &)> //
                                          >
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~core_listener();

      public:
        core_listener(pw_core *);
        core_listener(core_listener &&) noexcept;

      public:
        void clear(core_event event);
        void remove(core_event event, std::uint64_t id);
    };
} // namespace pipewire
