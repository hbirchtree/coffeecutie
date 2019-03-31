#pragma once

#include "libc_types.h"
#include <coffee/core/types/debug/component.h>
#include <coffee/core/types/debug/severity.h>
#include <coffee/core/types/debug/type.h>
#include <coffee/core/types/display/gl_properties.h>
#include <coffee/core/types/display/monitor.h>
#include <coffee/core/types/hardware_info.h>
#include <coffee/core/types/point.h>
#include <coffee/core/types/size.h>
#include <coffee/core/types/software_info.h>
#include <coffee/core/types/version.h>

namespace Coffee {
namespace Strings {

/* Extension resolvers */

extern cstring to_string(debug::Component const& arg);

extern cstring to_string(debug::Type const& arg);

extern cstring to_string(debug::Severity const& arg);

extern CString to_string(version<u8>& arg);

extern CString to_string(version<i32> const& arg);
extern CString to_string(version<u32> const& arg);

extern CString to_string(HWDeviceInfo const& arg);

extern CString to_string(SWVersionInfo const& arg);

extern CString to_string(const Display::GL::Version& arg);

extern CString to_string(Display::ContextBits const& arg);

extern CString to_string(Display::Monitor const& arg);

template<typename T>
inline CString to_string(point_2d<T> const& v)
{
    return cStringFormat("p({0},{1})", v.x, v.y);
}

template<typename T>
inline CString to_string(size_2d<T> const& v)
{
    return cStringFormat("sz({0},{1})", v.w, v.h);
}

inline CString to_string(Size const& v)
{
    return cStringFormat("sz({0},{1})", v.w, v.h);
}

template<typename T>
inline CString to_string(size_3d<T> const& v)
{
    return cStringFormat("sz({0},{1},{2}})", v.width, v.height, v.depth);
}

} // namespace Strings
} // namespace Coffee
