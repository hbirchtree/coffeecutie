#pragma once

#include "extensionresolvers.h"
#include <coffee/core/base/types/cdisplay.h>
#include <coffee/core/types/edef/dbgenum.h>

namespace Coffee {
namespace Strings {

/* Extension resolvers */

extern cstring to_string(debug::Component const& arg);

extern cstring to_string(debug::Type const& arg);

extern cstring to_string(debug::Severity const& arg);

extern CString to_string(_cbasic_version<u8>& arg);

extern CString to_string(_cbasic_version<i32> const& arg);
extern CString to_string(_cbasic_version<u32> const& arg);

extern CString to_string(HWDeviceInfo const& arg);

extern CString to_string(SWVersionInfo const& arg);

extern CString to_string(Display::CGLVersion const& arg);

extern CString to_string(Display::CDContextBits const& arg);

extern CString to_string(Display::CDMonitor const& arg);

template<typename T>
inline CString to_string(_cbasic_point_2d<T> const& v)
{
    return cStringFormat("p({0},{1})", v.x, v.y);
}

template<typename T>
inline CString to_string(_cbasic_size_2d<T> const& v)
{
    return cStringFormat("sz({0},{1})", v.w, v.h);
}

inline CString to_string(CSize const& v)
{
    return cStringFormat("sz({0},{1})", v.w, v.h);
}

template<typename T>
inline CString to_string(_cbasic_size_3d<T> const& v)
{
    return cStringFormat("sz({0},{1},{2}})", v.width, v.height, v.depth);
}

} // namespace Strings
} // namespace Coffee
