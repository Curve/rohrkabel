#pragma once
#include "main.hpp"

#include <future>

namespace pipewire
{
    template <typename Function> auto main_loop::call_safe(Function &&function)
    {
        using function_t = decltype(std::function(function));
        using return_t = typename function_t::result_type;
        std::promise<return_t> rtn;

        m_queue_mutex.lock();
        m_queue.push([&rtn, function = std::forward<Function>(function)] {
            if constexpr (std::is_same_v<return_t, void>)
            {
                function();
                rtn.set_value();
            }
            else
            {
                rtn.set_value(function());
            }
        });
        m_queue_mutex.unlock();

        emit_event();

        return rtn.get_future().get();
    }
} // namespace pipewire