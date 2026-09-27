#pragma once

#if defined(__cpp_lib_reflection) && defined(__cpp_impl_reflection) && !defined(__clang_analyzer__)
#include <meta>
#endif
#include <string>

namespace stl_types {
#if defined(__cpp_lib_reflection) && defined(__cpp_impl_reflection) && !defined(__clang_analyzer__)
constexpr bool flag_print_supported = true;

template<typename E>
std::string flags_to_string(E flags)
{
    std::string out;
    template for(constexpr auto val : std::define_static_array(std::meta::enumerators_of(^^E)))
    {
        if((flags & [:val:]) != static_cast<E>(0))
        {
            out.append(std::meta::identifier_of(val));
            out.append(" | ");
        }
    }
    if(out.empty())
        out = "none";
    else
        out.resize(out.size() - 3);
    return fmt::format("[{}]", out);
}
#else
constexpr bool flag_print_supported = false;

template<typename E>
std::string flags_to_string(E flags)
{
    return {};
}
#endif
}
