#pragma once
#include <memory>
#include "main_loop.hpp"

struct pw_context;
namespace pipewire
{
    class context
    {
        struct impl;

      private:
        main_loop &m_main_loop;
        std::unique_ptr<impl> m_impl;

      public:
        ~context();
        context(main_loop &);

      public:
        [[nodiscard]] pw_context *get() const;
        [[nodiscard]] main_loop &get_main_loop();
    };
} // namespace pipewire