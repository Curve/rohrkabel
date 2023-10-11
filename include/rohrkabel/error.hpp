#pragma once

namespace pipewire
{
    struct error
    {
        int seq;
        int res;
        const char *message;
    };
} // namespace pipewire
