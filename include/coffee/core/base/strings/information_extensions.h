#pragma once

#include "extensionresolvers.h"
#include <coffee/core/base/types/cdisplay.h>
#include <coffee/core/types/edef/dbgenum.h>

namespace Coffee{
namespace Strings{

/* Extension resolvers */

extern cstring to_string(DebugComponent const& arg);

extern cstring to_string(DebugType const& arg);

extern cstring to_string(Severity const& arg);

extern CString to_string(_cbasic_version<uint8>& arg);

extern CString to_string(_cbasic_version<int32> const& arg);
extern CString to_string(_cbasic_version<uint32> const& arg);

extern CString to_string(HWDeviceInfo const& arg);

extern CString to_string(SWVersionInfo const& arg);

extern CString to_string(Display::CGLVersion const& arg);

extern CString to_string(Display::CDContextBits const& arg);

template<typename T>
inline CString to_string(_cbasic_point_2d<T> v)
{
    return cStringFormat("p({0},{1})", v.x,v.y);
}

template<typename T>
inline CString to_string(_cbasic_size_2d<T> v)
{
    return cStringFormat("sz({0},{1})", v.w,v.h);
}

template<typename T>
inline CString to_string(_cbasic_size_3d<T> v)
{
    return cStringFormat("sz({0},{1},{2}})", v.width,v.height,v.depth);
}

}
}
