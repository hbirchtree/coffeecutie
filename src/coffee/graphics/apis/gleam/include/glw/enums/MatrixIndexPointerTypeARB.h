#pragma once

#include "common.h"

namespace gl::group {

// MatrixIndexPointerTypeARB
enum class matrix_index_pointer_type_arb : u32
{
    unsigned_byte  = 0x1401, // GL_UNSIGNED_BYTE
    unsigned_int   = 0x1405, // GL_UNSIGNED_INT
    unsigned_short = 0x1403, // GL_UNSIGNED_SHORT
}; // enum class matrix_index_pointer_type_arb

} // namespace gl::group
