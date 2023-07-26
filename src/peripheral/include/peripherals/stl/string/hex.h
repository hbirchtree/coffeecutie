#pragma once

#include "convert.h"
#include "pad.h"

#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>

namespace stl_types::str::fmt {

template<typename CharType = char>
FORCEDINLINE std::basic_string<CharType> hexify(libc_types::u64 s)
{
    if(s == 0)
        return "0";

    long long unsigned int ss = s;

    std::basic_string<CharType> str;
    str.resize(static_cast<size_t>(snprintf(nullptr, 0, "%llx", ss)) + 1);
    snprintf(&str[0], str.size() + 1, "%llx", ss);
    str.resize(str.size() - 1);
    stl_types::str::trim::left_zero(str);
    return str;
}

template<typename CharType = char>
FORCEDINLINE std::basic_string<CharType> pointer_pad(
    libc_types::u64 ptr, libc_types::u32 pad = 0)
{
    return pad::left<CharType>(hexify(ptr), '0', pad);
}

template<typename CharType = char>
FORCEDINLINE std::basic_string<CharType> pointerify(libc_types::u64 const& ptr)
{
    return "0x" + hexify(ptr);
}
template<
    typename T,
    typename CharType                                               = char,
    typename std::enable_if<std::is_pointer<T>::value, bool>::type* = nullptr>
FORCEDINLINE std::basic_string<CharType> pointerify(T ptr)
{
    return pointerify(C_FCAST<libc_types::u64>(ptr));
}

template<typename CharType = char>
FORCEDINLINE std::basic_string<CharType> hexdump(
    semantic::mem_chunk<const char> const& data,
    bool                                   spacing      = true,
    size_t                                 newline_freq = 0)
{
    std::basic_string<CharType> out;
    out.reserve(data.size * 2 /* Hexadec */ + data.size * spacing /* Space */);

    for(size_t i = 0; i < data.size; i++)
    {
        out.append("00");
        sprintf(&out[out.size() - 2], "%02x", data[i]);
        if(newline_freq == 0 || (i + 1) % newline_freq != 0)
        {
            if(spacing)
                out.append(" ");
        } else
            out.append("\n");
    }

    return out;
}

} // namespace stl_types::str::fmt

namespace hex {

FORCEDINLINE std::string encode(std::string const& from)
{
    return stl_types::str::fmt::hexdump(
        semantic::mem_chunk<const char>::ofContainer(from), false);
}

} // namespace hex
