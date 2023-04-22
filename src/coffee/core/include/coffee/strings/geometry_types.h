#pragma once

#include <peripherals/typing/geometry/bounding_box.h>
#include <string>

namespace Coffee::Strings {

extern std::string to_string(
    typing::geometry::boundingbox<libc_types::i32> const& box);
}

