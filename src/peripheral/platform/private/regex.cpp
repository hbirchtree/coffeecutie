#include <plat/regex.h>

namespace platform {
namespace regex {
const char* regex_error_category::name() const noexcept
{
    return "regex_error";
}

std::string regex_error_category::message(int) const
{
    return {};
}
} // namespace regex
} // namespace platform
