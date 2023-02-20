#pragma once

#include "libc_types.h"
#include <coffee/core/types/debug/component.h>
#include <coffee/core/types/debug/severity.h>
#include <coffee/core/types/debug/type.h>
#include <coffee/core/types/point.h>
#include <coffee/core/types/size.h>

namespace Coffee {
namespace Strings {

/* Extension resolvers */

extern std::string_view to_string(debug::Component const& arg);

extern std::string_view to_string(debug::Type const& arg);

extern std::string_view to_string(debug::Severity const& arg);

template<typename T>
inline CString to_string(point_2d<T> const& v)
{
    return fmt("p({0},{1})", v.x, v.y);
}

template<typename T>
inline CString to_string(size_2d<T> const& v)
{
    return fmt("sz({0},{1})", v.w, v.h);
}

inline CString to_string(Size const& v)
{
    return fmt("sz({0},{1})", v.w, v.h);
}

template<typename T>
inline CString to_string(size_3d<T> const& v)
{
    return fmt("sz({0},{1},{2}})", v.width, v.height, v.depth);
}

} // namespace Strings
} // namespace Coffee
