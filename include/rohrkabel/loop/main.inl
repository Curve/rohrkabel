#pragma once
#include "main.hpp"
#include "../utils/proxy/proxy.hpp"

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
                rtn.set_value(std::move(function()));
            }
        });
        emit_event();
        m_queue_mutex.unlock();

        if constexpr (std::is_base_of_v<proxy, return_t>)
        {
            std::lock_guard guard(m_proxy_mutex);
            auto shared_rtn = std::make_shared<return_t>(std::move(rtn.get_future().get()));

            m_proxies.emplace_back(shared_rtn);
            return safe_proxy(shared_rtn, *this);
        }
        else
        {
            return rtn.get_future().get();
        }
    }
} // namespace pipewire