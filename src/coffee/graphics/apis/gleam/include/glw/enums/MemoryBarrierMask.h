#pragma once

#include "common.h"

namespace gl::group {

// MemoryBarrierMask
enum class memory_barrier_mask : u32
{
    all_barrier_bits           = 0xFFFFFFFF, // GL_ALL_BARRIER_BITS
    atomic_counter_barrier_bit = 0x00001000, // GL_ATOMIC_COUNTER_BARRIER_BIT
    buffer_update_barrier_bit  = 0x00000200, // GL_BUFFER_UPDATE_BARRIER_BIT
    client_mapped_buffer_barrier_bit =
        0x00004000, // GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT
    command_barrier_bit       = 0x00000040, // GL_COMMAND_BARRIER_BIT
    element_array_barrier_bit = 0x00000002, // GL_ELEMENT_ARRAY_BARRIER_BIT
    framebuffer_barrier_bit   = 0x00000400, // GL_FRAMEBUFFER_BARRIER_BIT
    pixel_buffer_barrier_bit  = 0x00000080, // GL_PIXEL_BUFFER_BARRIER_BIT
    query_buffer_barrier_bit  = 0x00008000, // GL_QUERY_BUFFER_BARRIER_BIT
    shader_image_access_barrier_bit =
        0x00000020, // GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
    shader_storage_barrier_bit = 0x00002000, // GL_SHADER_STORAGE_BARRIER_BIT
    texture_fetch_barrier_bit  = 0x00000008, // GL_TEXTURE_FETCH_BARRIER_BIT
    texture_update_barrier_bit = 0x00000100, // GL_TEXTURE_UPDATE_BARRIER_BIT
    transform_feedback_barrier_bit =
        0x00000800,                   // GL_TRANSFORM_FEEDBACK_BARRIER_BIT
    uniform_barrier_bit = 0x00000004, // GL_UNIFORM_BARRIER_BIT
    vertex_attrib_array_barrier_bit =
        0x00000001,                    // GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT
    all_barrier_bits_ext = 0xFFFFFFFF, // GL_ALL_BARRIER_BITS_EXT
    atomic_counter_barrier_bit_ext =
        0x00001000, // GL_ATOMIC_COUNTER_BARRIER_BIT_EXT
    buffer_update_barrier_bit_ext =
        0x00000200, // GL_BUFFER_UPDATE_BARRIER_BIT_EXT
    client_mapped_buffer_barrier_bit_ext =
        0x00004000, // GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT_EXT
    command_barrier_bit_ext = 0x00000040, // GL_COMMAND_BARRIER_BIT_EXT
    element_array_barrier_bit_ext =
        0x00000002, // GL_ELEMENT_ARRAY_BARRIER_BIT_EXT
    framebuffer_barrier_bit_ext = 0x00000400, // GL_FRAMEBUFFER_BARRIER_BIT_EXT
    pixel_buffer_barrier_bit_ext =
        0x00000080, // GL_PIXEL_BUFFER_BARRIER_BIT_EXT
    shader_global_access_barrier_bit_nv =
        0x00000010, // GL_SHADER_GLOBAL_ACCESS_BARRIER_BIT_NV
    shader_image_access_barrier_bit_ext =
        0x00000020, // GL_SHADER_IMAGE_ACCESS_BARRIER_BIT_EXT
    texture_fetch_barrier_bit_ext =
        0x00000008, // GL_TEXTURE_FETCH_BARRIER_BIT_EXT
    texture_update_barrier_bit_ext =
        0x00000100, // GL_TEXTURE_UPDATE_BARRIER_BIT_EXT
    transform_feedback_barrier_bit_ext =
        0x00000800, // GL_TRANSFORM_FEEDBACK_BARRIER_BIT_EXT
    uniform_barrier_bit_ext = 0x00000004, // GL_UNIFORM_BARRIER_BIT_EXT
    vertex_attrib_array_barrier_bit_ext =
        0x00000001, // GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT_EXT
}; // enum class memory_barrier_mask
C_FLAGS(memory_barrier_mask, u32);

} // namespace gl::group
