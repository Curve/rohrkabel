#pragma once

#include "module.hpp"
#include "../../utils/listener.hpp"

#include <memory>

namespace pipewire::impl
{
    enum class module_event : std::uint8_t
    {
        destroy,
        free,
        initialized,
        registered,
    };

    class module_listener : public listener<ereignis::event<module_event::destroy, void()>,     //
                                            ereignis::event<module_event::free, void()>,        //
                                            ereignis::event<module_event::initialized, void()>, //
                                            ereignis::event<module_event::registered, void()>   //
                                            >
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        module_listener(module::raw_type *);

      public:
        ~module_listener();

      public:
        static const std::uint32_t version;
    };
} // namespace pipewire::impl
