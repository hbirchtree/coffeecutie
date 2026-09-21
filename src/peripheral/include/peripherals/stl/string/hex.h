#pragma once

#include "convert.h"
#include "pad.h"

#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/type_safety.h>

#include <string>
#include <type_traits>

/* Kept deliberately thin: glw/base.h pulls this in, and through it most of
 * glw/. Anything needing semantic::mem_chunk belongs in hexdump.h. */

namespace stl_types::str::fmt {

/* Defined in private/string_encoding.cpp, to keep <fmt/format.h> out. */
std::string hexify(libc_types::u64 s);

FORCEDINLINE std::string pointer_pad(
    libc_types::u64 ptr, libc_types::u32 pad = 0)
{
    return pad::left<char>(hexify(ptr), '0', pad);
}

FORCEDINLINE std::string pointerify(libc_types::u64 const& ptr)
{
    return "0x" + hexify(ptr);
}

template<
    typename T,
    typename std::enable_if<std::is_pointer<T>::value, bool>::type* = nullptr>
FORCEDINLINE std::string pointerify(T ptr)
{
    return pointerify(C_FCAST<libc_types::u64>(ptr));
}

} // namespace stl_types::str::fmt
