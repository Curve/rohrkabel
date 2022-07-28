#pragma once
#include <memory>
#include "loop/main.hpp"

struct pw_context;
namespace pipewire
{
    class context
    {
        struct impl;

      private:
        main_loop &m_loop;
        std::unique_ptr<impl> m_impl;

      public:
        ~context();
        context(main_loop &);

      public:
        [[nodiscard]] main_loop &get_loop();
        [[nodiscard]] pw_context *get() const;
    };
} // namespace pipewire