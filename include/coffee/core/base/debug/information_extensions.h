#pragma once

#include "extensionresolvers.h"

#include "../types/cdisplay.h"
#include "../../types/edef/dbgenum.h"

namespace Coffee{
namespace DebugFun{

/* Extension resolvers */

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        DebugComponent const& arg);

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        DebugType const& arg);

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        Severity const& arg);

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        const _cbasic_version<uint8>& arg);

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        const _cbasic_version<int32>& arg);

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        const _cbasic_version<uint32>& arg);

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        HWDeviceInfo const& arg);

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        SWVersionInfo const& arg);

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        Display::CGLVersion const& arg);

template<typename T>
inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        _cbasic_point_2d<T> v)
{
    return extArgReplace(fmt,index,cStringFormat("p({0},{1})",
                                               v.x,v.y));
}

template<typename T>
inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        _cbasic_size_2d<T> v)
{
    return extArgReplace(fmt,index,cStringFormat("sz({0},{1})",
                                               v.w,v.h));
}

template<typename T>
inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        _cbasic_size_3d<T> v)
{
    return extArgReplace(fmt,index,cStringFormat("sz({0},{1},{2}})",
                                               v.width,v.height,v.depth));
}

}
}
