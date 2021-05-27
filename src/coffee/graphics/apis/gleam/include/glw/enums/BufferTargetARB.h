#pragma once

#include "common.h"

namespace gl::groups {

// BufferTargetARB
enum class buffer_target_arb : ::libc_types::u32 {
#ifdef GL_ARRAY_BUFFER
    array_buffer = GL_ARRAY_BUFFER,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER
    atomic_counter_buffer = GL_ATOMIC_COUNTER_BUFFER,
#endif
#ifdef GL_COPY_READ_BUFFER
    copy_read_buffer = GL_COPY_READ_BUFFER,
#endif
#ifdef GL_COPY_WRITE_BUFFER
    copy_write_buffer = GL_COPY_WRITE_BUFFER,
#endif
#ifdef GL_DISPATCH_INDIRECT_BUFFER
    dispatch_indirect_buffer = GL_DISPATCH_INDIRECT_BUFFER,
#endif
#ifdef GL_DRAW_INDIRECT_BUFFER
    draw_indirect_buffer = GL_DRAW_INDIRECT_BUFFER,
#endif
#ifdef GL_ELEMENT_ARRAY_BUFFER
    element_array_buffer = GL_ELEMENT_ARRAY_BUFFER,
#endif
#ifdef GL_PARAMETER_BUFFER
    parameter_buffer = GL_PARAMETER_BUFFER,
#endif
#ifdef GL_PIXEL_PACK_BUFFER
    pixel_pack_buffer = GL_PIXEL_PACK_BUFFER,
#endif
#ifdef GL_PIXEL_UNPACK_BUFFER
    pixel_unpack_buffer = GL_PIXEL_UNPACK_BUFFER,
#endif
#ifdef GL_QUERY_BUFFER
    query_buffer = GL_QUERY_BUFFER,
#endif
#ifdef GL_SHADER_STORAGE_BUFFER
    shader_storage_buffer = GL_SHADER_STORAGE_BUFFER,
#endif
#ifdef GL_TEXTURE_BUFFER
    texture_buffer = GL_TEXTURE_BUFFER,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER
    transform_feedback_buffer = GL_TRANSFORM_FEEDBACK_BUFFER,
#endif
#ifdef GL_UNIFORM_BUFFER
    uniform_buffer = GL_UNIFORM_BUFFER,
#endif
}; // enum class buffer_target_arb

} // namespace gl::groups
