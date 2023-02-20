#pragma once

#include <string_view>

#if defined(__cpp_lib_source_location) && __cpp_lib_source_location > 201907L
#include <source_location>
#endif

namespace stl_types {

#if defined(__cpp_lib_source_location) && __cpp_lib_source_location > 201907L
using std::source_location;
#else
struct source_location
{
    source_location(
        std::string_view file     = __builtin_FILE(),
        int              line     = __builtin_LINE(),
        std::string_view function = __builtin_FUNCTION()) :
        m_file(file),
        m_line(line), m_function(function)
    {
    }

    std::string_view file_name() const { return m_file; }
    int line() const { return m_line; }
    std::string_view function_name() const { return m_function; }

    std::string_view m_file;
    int              m_line;
    std::string_view m_function;
};
#endif

} // namespace stl_types
