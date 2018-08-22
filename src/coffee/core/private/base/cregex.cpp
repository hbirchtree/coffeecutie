#include <coffee/core/base/textprocessing/cregex.h>

namespace Coffee {
namespace Regex {
const char* regex_error_category::name() const noexcept
{
    return "regex_error";
}

std::string regex_error_category::message(int) const
{
    return {};
}
} // namespace Regex
} // namespace Coffee
