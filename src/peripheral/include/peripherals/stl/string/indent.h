#pragma once

#include "replace.h"

namespace stl_types::str::fmt {

template<typename CharType = char>
FORCEDINLINE std::basic_string<CharType> indent(
    std::basic_string_view<CharType> const& in,
    std::basic_string_view<CharType> const& padding)
{
    std::basic_string<CharType> out = replace::str(in, "\n", padding);
    out.insert(out.begin(), padding.begin(), padding.end());
    return out;
}

} // namespace stl_types::str::fmt
