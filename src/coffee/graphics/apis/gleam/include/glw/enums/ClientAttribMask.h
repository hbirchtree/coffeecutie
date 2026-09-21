#pragma once

#include "common.h"

namespace gl::group {

// ClientAttribMask
enum class client_attrib_mask : u32
{
    client_all_attrib_bits  = 0xFFFFFFFF, // GL_CLIENT_ALL_ATTRIB_BITS
    client_pixel_store_bit  = 0x00000001, // GL_CLIENT_PIXEL_STORE_BIT
    client_vertex_array_bit = 0x00000002, // GL_CLIENT_VERTEX_ARRAY_BIT
}; // enum class client_attrib_mask
C_FLAGS(client_attrib_mask, u32);

} // namespace gl::group
