#pragma once

#include "common.h"

namespace gl::group {

// DataTypeEXT
enum class data_type_ext : ::libc_types::u32
{
#ifdef GL_MATRIX_EXT
    matrix_ext = GL_MATRIX_EXT,
#endif
#ifdef GL_SCALAR_EXT
    scalar_ext = GL_SCALAR_EXT,
#endif
#ifdef GL_VECTOR_EXT
    vector_ext = GL_VECTOR_EXT,
#endif
}; // enum class data_type_ext

} // namespace gl::group
