#pragma once

#include "common.h"

namespace gl::group {

// MemoryBarrierMask
enum class memory_barrier_mask : u32
{
#ifdef GL_ALL_BARRIER_BITS
    all_barrier_bits = GL_ALL_BARRIER_BITS,
#endif
#ifdef GL_ALL_BARRIER_BITS_EXT
    all_barrier_bits_ext = GL_ALL_BARRIER_BITS_EXT,
#endif
#ifdef GL_ATOMIC_COUNTER_BARRIER_BIT
    atomic_counter_barrier_bit = GL_ATOMIC_COUNTER_BARRIER_BIT,
#endif
#ifdef GL_ATOMIC_COUNTER_BARRIER_BIT_EXT
    atomic_counter_barrier_bit_ext = GL_ATOMIC_COUNTER_BARRIER_BIT_EXT,
#endif
#ifdef GL_BUFFER_UPDATE_BARRIER_BIT
    buffer_update_barrier_bit = GL_BUFFER_UPDATE_BARRIER_BIT,
#endif
#ifdef GL_BUFFER_UPDATE_BARRIER_BIT_EXT
    buffer_update_barrier_bit_ext = GL_BUFFER_UPDATE_BARRIER_BIT_EXT,
#endif
#ifdef GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT
    client_mapped_buffer_barrier_bit = GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT,
#endif
#ifdef GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT_EXT
    client_mapped_buffer_barrier_bit_ext =
        GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT_EXT,
#endif
#ifdef GL_COMMAND_BARRIER_BIT
    command_barrier_bit = GL_COMMAND_BARRIER_BIT,
#endif
#ifdef GL_COMMAND_BARRIER_BIT_EXT
    command_barrier_bit_ext = GL_COMMAND_BARRIER_BIT_EXT,
#endif
#ifdef GL_ELEMENT_ARRAY_BARRIER_BIT
    element_array_barrier_bit = GL_ELEMENT_ARRAY_BARRIER_BIT,
#endif
#ifdef GL_ELEMENT_ARRAY_BARRIER_BIT_EXT
    element_array_barrier_bit_ext = GL_ELEMENT_ARRAY_BARRIER_BIT_EXT,
#endif
#ifdef GL_FRAMEBUFFER_BARRIER_BIT
    framebuffer_barrier_bit = GL_FRAMEBUFFER_BARRIER_BIT,
#endif
#ifdef GL_FRAMEBUFFER_BARRIER_BIT_EXT
    framebuffer_barrier_bit_ext = GL_FRAMEBUFFER_BARRIER_BIT_EXT,
#endif
#ifdef GL_PIXEL_BUFFER_BARRIER_BIT
    pixel_buffer_barrier_bit = GL_PIXEL_BUFFER_BARRIER_BIT,
#endif
#ifdef GL_PIXEL_BUFFER_BARRIER_BIT_EXT
    pixel_buffer_barrier_bit_ext = GL_PIXEL_BUFFER_BARRIER_BIT_EXT,
#endif
#ifdef GL_QUERY_BUFFER_BARRIER_BIT
    query_buffer_barrier_bit = GL_QUERY_BUFFER_BARRIER_BIT,
#endif
#ifdef GL_SHADER_GLOBAL_ACCESS_BARRIER_BIT_NV
    shader_global_access_barrier_bit_nv =
        GL_SHADER_GLOBAL_ACCESS_BARRIER_BIT_NV,
#endif
#ifdef GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
    shader_image_access_barrier_bit = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT,
#endif
#ifdef GL_SHADER_IMAGE_ACCESS_BARRIER_BIT_EXT
    shader_image_access_barrier_bit_ext =
        GL_SHADER_IMAGE_ACCESS_BARRIER_BIT_EXT,
#endif
#ifdef GL_SHADER_STORAGE_BARRIER_BIT
    shader_storage_barrier_bit = GL_SHADER_STORAGE_BARRIER_BIT,
#endif
#ifdef GL_TEXTURE_FETCH_BARRIER_BIT
    texture_fetch_barrier_bit = GL_TEXTURE_FETCH_BARRIER_BIT,
#endif
#ifdef GL_TEXTURE_FETCH_BARRIER_BIT_EXT
    texture_fetch_barrier_bit_ext = GL_TEXTURE_FETCH_BARRIER_BIT_EXT,
#endif
#ifdef GL_TEXTURE_UPDATE_BARRIER_BIT
    texture_update_barrier_bit = GL_TEXTURE_UPDATE_BARRIER_BIT,
#endif
#ifdef GL_TEXTURE_UPDATE_BARRIER_BIT_EXT
    texture_update_barrier_bit_ext = GL_TEXTURE_UPDATE_BARRIER_BIT_EXT,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BARRIER_BIT
    transform_feedback_barrier_bit = GL_TRANSFORM_FEEDBACK_BARRIER_BIT,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BARRIER_BIT_EXT
    transform_feedback_barrier_bit_ext = GL_TRANSFORM_FEEDBACK_BARRIER_BIT_EXT,
#endif
#ifdef GL_UNIFORM_BARRIER_BIT
    uniform_barrier_bit = GL_UNIFORM_BARRIER_BIT,
#endif
#ifdef GL_UNIFORM_BARRIER_BIT_EXT
    uniform_barrier_bit_ext = GL_UNIFORM_BARRIER_BIT_EXT,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT
    vertex_attrib_array_barrier_bit = GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT_EXT
    vertex_attrib_array_barrier_bit_ext =
        GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT_EXT,
#endif
}; // enum class memory_barrier_mask
C_FLAGS(memory_barrier_mask, u32);

} // namespace gl::group
