#include "loop/loop.hpp"

#include <mutex>
#include <pipewire/pipewire.h>

namespace pipewire
{
    struct loop::impl
    {
        static inline std::once_flag flag;
    };

    loop::loop() : m_impl(std::make_unique<impl>())
    {
        std::call_once(impl::flag, [] { pw_init(nullptr, nullptr); });
    }

    loop::~loop() = default;
} // namespace pipewire