#pragma once

#include "utils/res.hpp"
#include "utils/deleter.hpp"

#include <memory>
#include <system_error>

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
        [[nodiscard]] pw_loop *loop() const;

      public:
        [[nodiscard]] raw_type *get() const;
        [[nodiscard]] raw_type *release() &&;

      public:
        [[nodiscard]] operator raw_type *() const &;
        [[nodiscard]] operator raw_type *() const && = delete;

      public:
        [[nodiscard]] static res<std::shared_ptr<main_loop>, std::error_code> create();

      public:
        [[nodiscard]] static std::shared_ptr<main_loop> from(raw_type *);
        [[nodiscard]] static std::shared_ptr<main_loop> view(raw_type *);
    };
} // namespace pipewire
