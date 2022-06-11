#pragma once
#include <string>
#include <stdexcept>

namespace pipewire
{
    class error : std::exception
    {
        int m_seq, m_res;
        const char *m_message;

      public:
        error(int seq, int res, const char *message);

      public:
        int seq() const noexcept;
        int res() const noexcept;
        const char *what() const noexcept override;
    };
} // namespace pipewire