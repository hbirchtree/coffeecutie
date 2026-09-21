#pragma once

#include <peripherals/stl/constructors.h>

namespace stl_types {

struct non_copy
{
    non_copy() = default;
    C_MOVE_CONSTRUCTOR(non_copy);
    C_DELETE_COPY_CONSTRUCTOR(non_copy);
};

} // namespace stl_types
