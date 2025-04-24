#include "utils/check.hpp"

#include <print>
#include <cstring>

namespace pipewire
{
#ifdef NDEBUG
    void check(bool, std::string_view, const std::source_location) {}
#else
    void check(bool condition, std::string_view message, const std::source_location loc)
    {
        if (condition)
        {
            return;
        }

        const auto *error       = strerror(errno); // NOLINT(*-mt-unsafe)
        auto [file, line, func] = std::make_tuple(loc.file_name(), loc.line(), loc.function_name());
        std::println(stderr, R"([{}] ({}:{}): check failed "{}", error is "{}")", func, file, line, message, error);
    }
#endif
} // namespace pipewire
