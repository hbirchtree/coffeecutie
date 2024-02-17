#include <fmt_extensions/info.h>

#include <magic_enum.hpp>

namespace semantic::detail {

std::string_view enum_to_string(semantic::debug::Component component)
{
    return magic_enum::enum_name(component);
}

std::string_view enum_to_string(semantic::debug::Severity severity)
{
    return magic_enum::enum_name(severity);
}

std::string_view enum_to_string(semantic::debug::Type type)
{
    return magic_enum::enum_name(type);
}

} // namespace semantic::detail
