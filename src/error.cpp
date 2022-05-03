#include "error.hpp"

namespace pipewire
{
    error::error(int seq, int res, const char *message) : m_seq(seq), m_res(res), m_message(message) {}

    int error::seq() const noexcept
    {
        return m_seq;
    }

    int error::res() const noexcept
    {
        return m_res;
    }

    const char *error::what() const noexcept
    {
        return m_message;
    }
} // namespace pipewire