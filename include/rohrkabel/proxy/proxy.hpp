#pragma once

#include "../spa/dict.hpp"
#include "../utils/task.hpp"
#include "../utils/traits.hpp"
#include "../utils/deleter.hpp"

#include <memory>
#include <string>

#include <cstdint>

struct pw_proxy;

namespace pipewire
{
    class proxy_listener;

    class proxy
    {
        struct impl;

      public:
        using raw_type = pw_proxy;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        proxy(deleter<raw_type>, raw_type *, spa::dict);

      public:
        proxy(proxy &&) noexcept;
        proxy &operator=(proxy &&) noexcept;

      public:
        virtual ~proxy();

      public:
        [[nodiscard]] std::uint32_t id() const;
        [[nodiscard]] spa::dict props() const;

      public:
        [[nodiscard]] lazy<int> sync() const;
        [[nodiscard]] int sync(int seq) const;

      public:
        [[nodiscard]] std::string type() const;
        [[nodiscard]] std::uint32_t version() const;

      public:
        template <detail::listener<raw_type> Listener = proxy_listener>
        [[nodiscard]] Listener listen() const;

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] raw_type *release() &&;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        static task<proxy> bind(raw_type *);

      public:
        static proxy from(raw_type *, spa::dict = {});
        static proxy view(raw_type *, spa::dict = {});
    };
} // namespace pipewire

#include "proxy.inl"
