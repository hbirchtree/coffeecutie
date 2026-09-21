#pragma once

#include "common.h"

namespace gl::group {

// UniformPName
enum class uniform_prop : u32
{
    uniform_array_stride = 0x8A3C, // GL_UNIFORM_ARRAY_STRIDE
    uniform_atomic_counter_buffer_index =
        0x92DA,                     // GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX
    uniform_block_index   = 0x8A3A, // GL_UNIFORM_BLOCK_INDEX
    uniform_is_row_major  = 0x8A3E, // GL_UNIFORM_IS_ROW_MAJOR
    uniform_matrix_stride = 0x8A3D, // GL_UNIFORM_MATRIX_STRIDE
    uniform_name_length   = 0x8A39, // GL_UNIFORM_NAME_LENGTH
    uniform_offset        = 0x8A3B, // GL_UNIFORM_OFFSET
    uniform_size          = 0x8A38, // GL_UNIFORM_SIZE
    uniform_type          = 0x8A37, // GL_UNIFORM_TYPE
}; // enum class uniform_prop

} // namespace gl::group
