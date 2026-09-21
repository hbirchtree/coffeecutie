#pragma once

#include "common.h"

namespace gl::group {

// BufferTargetARB
enum class buffer_target_arb : u32
{
    array_buffer              = 0x8892, // GL_ARRAY_BUFFER
    atomic_counter_buffer     = 0x92C0, // GL_ATOMIC_COUNTER_BUFFER
    copy_read_buffer          = 0x8F36, // GL_COPY_READ_BUFFER
    copy_write_buffer         = 0x8F37, // GL_COPY_WRITE_BUFFER
    dispatch_indirect_buffer  = 0x90EE, // GL_DISPATCH_INDIRECT_BUFFER
    draw_indirect_buffer      = 0x8F3F, // GL_DRAW_INDIRECT_BUFFER
    element_array_buffer      = 0x8893, // GL_ELEMENT_ARRAY_BUFFER
    parameter_buffer          = 0x80EE, // GL_PARAMETER_BUFFER
    pixel_pack_buffer         = 0x88EB, // GL_PIXEL_PACK_BUFFER
    pixel_unpack_buffer       = 0x88EC, // GL_PIXEL_UNPACK_BUFFER
    query_buffer              = 0x9192, // GL_QUERY_BUFFER
    shader_storage_buffer     = 0x90D2, // GL_SHADER_STORAGE_BUFFER
    texture_buffer            = 0x8C2A, // GL_TEXTURE_BUFFER
    transform_feedback_buffer = 0x8C8E, // GL_TRANSFORM_FEEDBACK_BUFFER
    uniform_buffer            = 0x8A11, // GL_UNIFORM_BUFFER
}; // enum class buffer_target_arb

} // namespace gl::group
