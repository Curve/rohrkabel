#pragma once

#include "utils/deleter.hpp"

#include <memory>

struct pw_loop;
struct pw_main_loop;

namespace pipewire
{
    class main_loop
    {
        struct impl;

      public:
        using raw_type = pw_main_loop;

      private:
        std::unique_ptr<impl> m_impl;

      private:
        main_loop(deleter<raw_type>, raw_type *);

      public:
        ~main_loop();

      public:
        void run() const;
        void quit() const;

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] pw_loop *loop() const;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[nodiscard]] static std::shared_ptr<main_loop> create();

      public:
        [[nodiscard]] static std::shared_ptr<main_loop> from(raw_type *);
        [[nodiscard]] static std::shared_ptr<main_loop> view(raw_type *);
    };
} // namespace pipewire
