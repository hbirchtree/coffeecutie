#pragma once

#include "common.h"

namespace gl::group {

// DataTypeEXT
enum class data_type_ext : u32
{
    matrix_ext = 0x87C0, // GL_MATRIX_EXT
    scalar_ext = 0x87BE, // GL_SCALAR_EXT
    vector_ext = 0x87BF, // GL_VECTOR_EXT
}; // enum class data_type_ext

} // namespace gl::group
