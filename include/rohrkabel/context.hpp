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

      private:
        std::unique_ptr<impl> m_impl;

      public:
        ~context();

      private:
        context();

      public:
        [[nodiscard]] std::shared_ptr<pipewire::core> core();

      public:
        [[nodiscard]] pw_context *get() const;
        [[nodiscard]] std::shared_ptr<main_loop> loop() const;

      public:
        [[nodiscard]] operator pw_context *() const &;
        [[nodiscard]] operator pw_context *() const && = delete;

      public:
        [[nodiscard]] static std::shared_ptr<context> create(std::shared_ptr<main_loop>);
    };
} // namespace pipewire
