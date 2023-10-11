#include "utils/check.hpp"

#include <format>
#include <cstring>
#include <iostream>

namespace pipewire
{
#ifdef NDEBUG
    void check(bool, const char *, const std::source_location) {}
#else
    void check(bool condition, const char *message, const std::source_location loc)
    {
        if (condition)
        {
            return;
        }

        const auto *error            = strerror(errno); // NOLINT(*-mt-unsafe)
        auto [file, line, col, func] = std::make_tuple(loc.file_name(), loc.line(), loc.column(), loc.function_name());

        std::cerr << std::format("{} ({}:{}) [{}]: check failed \"{}\", error is \"{}\"\n", file, line, col, func,
                                 message, error);
    }
#endif
} // namespace pipewire
