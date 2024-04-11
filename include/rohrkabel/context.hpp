#pragma once

#include "loop.hpp"

#include <memory>

struct pw_context;

namespace pipewire
{
    class core;

    class context : public std::enable_shared_from_this<context>
    {
        struct impl;

      public:
        using raw_type = pw_context;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~context();

      private:
        context();

      public:
        [[nodiscard]] std::shared_ptr<pipewire::core> core();

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] std::shared_ptr<main_loop> loop() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[nodiscard]] static std::shared_ptr<context> create(std::shared_ptr<main_loop>);
    };
} // namespace pipewire
