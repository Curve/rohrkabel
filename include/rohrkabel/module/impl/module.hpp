#pragma once

#include "../info.hpp"

#include "../../properties.hpp"
#include "../../utils/traits.hpp"
#include "../../utils/deleter.hpp"

#include <memory>
#include <optional>

struct pw_impl_module;

namespace pipewire
{
    struct module_options;

    struct loopback_options
    {
        struct
        {
            std::string name;
            std::string description;
            std::optional<std::uint32_t> node;
        } capture;

        struct
        {
            std::string name;
            std::string description;
            std::optional<std::uint32_t> node;
        } playback;

      public:
        operator module_options() const;
    };
} // namespace pipewire

namespace pipewire::impl
{
    class module_listener;

    class mod
    {
        struct impl;

      public:
        using raw_type = pw_impl_module;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        mod(deleter<raw_type>, raw_type *);

      public:
        mod(mod &&) noexcept;
        mod &operator=(mod &&) noexcept;

      public:
        ~mod();

      public:
        [[nodiscard]] module_info info() const;
        [[nodiscard]] pipewire::properties properties() const;

      public:
        void update_properties(pipewire::properties);

      public:
        template <detail::listener<raw_type> Listener = module_listener>
        [[nodiscard]] Listener listen() const;

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] raw_type *release() &&;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        static mod from(raw_type *);
        static mod view(raw_type *);
    };
} // namespace pipewire::impl

#include "module.inl"
