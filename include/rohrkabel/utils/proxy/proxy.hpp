#pragma once
#include "../../proxy.hpp"
#include "../../loop/main.hpp"

#include <memory>

namespace pipewire
{
    template <typename T, typename = std::enable_if_t<std::is_base_of_v<proxy, T>>> class safe_proxy
    {
      private:
        main_loop &m_loop;
        std::shared_ptr<T> m_instance;

      public:
        ~safe_proxy();
        explicit safe_proxy(std::shared_ptr<T> instance, main_loop &loop);

      public:
        safe_proxy(const safe_proxy &) = delete;
        safe_proxy(safe_proxy &&) noexcept = delete;

      public:
        const T *operator->() const;

      public:
        T &get_safe();
    };
} // namespace pipewire

#include "proxy.inl"