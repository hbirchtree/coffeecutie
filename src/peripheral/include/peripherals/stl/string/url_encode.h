#pragma once

#include <peripherals/identify/compiler/function_inlining.h>

#include <string>
#include <string_view>

namespace stl_types::str::url_encode {

namespace detail {

FORCEDINLINE constexpr bool is_unreserved(char c)
{
    // RFC 3986 2.3 unreserved characters -- everything else gets encoded.
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' ||
           c == '~';
}

FORCEDINLINE constexpr char hex_digit(unsigned nibble)
{
    return static_cast<char>(nibble < 10 ? '0' + nibble : 'A' + (nibble - 10));
}

} // namespace detail

FORCEDINLINE std::string encode(std::string_view src)
{
    std::string out;
    out.reserve(src.size());

    for(unsigned char c : src)
    {
        if(detail::is_unreserved(static_cast<char>(c)))
            out.push_back(static_cast<char>(c));
        else
        {
            out.push_back('%');
            out.push_back(detail::hex_digit((c >> 4) & 0xF));
            out.push_back(detail::hex_digit(c & 0xF));
        }
    }

    return out;
}

} // namespace stl_types::str::url_encode
