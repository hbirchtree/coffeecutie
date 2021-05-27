#pragma once

#include "common.h"

namespace gl::groups {

// UniformPName
enum class uniform_prop : ::libc_types::u32 {
#ifdef GL_UNIFORM_ARRAY_STRIDE
    uniform_array_stride = GL_UNIFORM_ARRAY_STRIDE,
#endif
#ifdef GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX
    uniform_atomic_counter_buffer_index = GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX,
#endif
#ifdef GL_UNIFORM_BLOCK_INDEX
    uniform_block_index = GL_UNIFORM_BLOCK_INDEX,
#endif
#ifdef GL_UNIFORM_IS_ROW_MAJOR
    uniform_is_row_major = GL_UNIFORM_IS_ROW_MAJOR,
#endif
#ifdef GL_UNIFORM_MATRIX_STRIDE
    uniform_matrix_stride = GL_UNIFORM_MATRIX_STRIDE,
#endif
#ifdef GL_UNIFORM_NAME_LENGTH
    uniform_name_length = GL_UNIFORM_NAME_LENGTH,
#endif
#ifdef GL_UNIFORM_OFFSET
    uniform_offset = GL_UNIFORM_OFFSET,
#endif
#ifdef GL_UNIFORM_SIZE
    uniform_size = GL_UNIFORM_SIZE,
#endif
#ifdef GL_UNIFORM_TYPE
    uniform_type = GL_UNIFORM_TYPE,
#endif
}; // enum class uniform_prop

} // namespace gl::groups
