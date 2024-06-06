#pragma once

#include "loop.hpp"
#include "utils/deleter.hpp"

#include <memory>

struct pw_context;

namespace pipewire
{
    class context
    {
        struct impl;

      public:
        using raw_type = pw_context;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~context();

      private:
        context(deleter<raw_type>, raw_type *, std::shared_ptr<main_loop>);

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] std::shared_ptr<main_loop> loop() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[nodiscard]] static std::shared_ptr<context> create(std::shared_ptr<main_loop>);

      public:
        [[nodiscard]] static std::shared_ptr<context> from(raw_type *, std::shared_ptr<main_loop>);
        [[nodiscard]] static std::shared_ptr<context> view(raw_type *, std::shared_ptr<main_loop>);
    };
} // namespace pipewire
