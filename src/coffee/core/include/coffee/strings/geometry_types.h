#pragma once

#include <coffee/core/stl_types.h>
#include <peripherals/typing/geometry/bounding_box.h>

namespace Coffee {
namespace Strings {

extern CString to_string(
    typing::geometry::boundingbox<libc_types::i32> const& box);
}
} // namespace Coffee
