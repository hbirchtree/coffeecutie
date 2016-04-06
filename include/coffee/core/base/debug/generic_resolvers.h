#pragma once

#include "../../coffee_mem_macros.h"
#include "../../plat/plat_quirks_toggling.h"
#include "../../plat/memory/string_ops.h"
#include "../../plat/memory/stlstring_ops.h"
#include "extensionresolvers.h"

namespace Coffee{
namespace DebugFun{

FORCEDINLINE
CString cStringReplace(
        CString const& fmt, size_t const& index,
        uint8 const& arg)
{
    return cStringReplace(fmt,index,Convert::uintctostring(arg));
}
FORCEDINLINE
CString cStringReplace(
        CString const& fmt, size_t const& index,
        int8 const& arg)
{
    return cStringReplace(fmt,index,Convert::intctostring(arg));
}

FORCEDINLINE
CString cStringReplace(
        CString const& fmt, size_t const& index,
        uint16 const& arg)
{
    return cStringReplace(fmt,index,Convert::uintstostring(arg));
}
FORCEDINLINE
CString cStringReplace(
        CString const& fmt, size_t const& index,
        int16 const& arg)
{
    return cStringReplace(fmt,index,Convert::intstostring(arg));
}

FORCEDINLINE
CString cStringReplace(
        CString const& fmt, size_t const& index,
        uint32 const& arg)
{
    return cStringReplace(fmt,index,Convert::uinttostring(arg));
}
FORCEDINLINE
CString cStringReplace(
        CString const& fmt, size_t const& index,
        int32 const& arg)
{
    return cStringReplace(fmt,index,Convert::inttostring(arg));
}

FORCEDINLINE
CString cStringReplace(
        CString const& fmt, size_t const& index,
        uint64 const& arg)
{
    return cStringReplace(fmt,index,Convert::uintltostring(arg));
}
FORCEDINLINE
CString cStringReplace(
        CString const& fmt, size_t const& index,
        int64 const& arg)
{
    return cStringReplace(fmt,index,Convert::intltostring(arg));
}
#ifdef COFFEE_ARCH_LLP64
FORCEDINLINE
CString cStringReplace(
        CString const& fmt, size_t const& index,
        long int const& arg)
{
    return cStringReplace(fmt,index,Convert::intltostring(arg));
}
#endif

}
}
