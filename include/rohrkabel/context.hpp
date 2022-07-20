#pragma once
#include <memory>
#include "loop/loop.hpp"

struct pw_context;
namespace pipewire
{
    class context
    {
        struct impl;

      private:
        loop &m_loop;
        std::unique_ptr<impl> m_impl;

      public:
        ~context();
        context(loop &);

      public:
        [[nodiscard]] loop &get_loop();
        [[nodiscard]] pw_context *get() const;
    };
} // namespace pipewire