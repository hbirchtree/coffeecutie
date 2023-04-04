#pragma once

#include "common.h"

namespace gl::group {

// ClientAttribMask
enum class client_attrib_mask : u32
{
#ifdef GL_CLIENT_ALL_ATTRIB_BITS
    client_all_attrib_bits = GL_CLIENT_ALL_ATTRIB_BITS,
#endif
#ifdef GL_CLIENT_PIXEL_STORE_BIT
    client_pixel_store_bit = GL_CLIENT_PIXEL_STORE_BIT,
#endif
#ifdef GL_CLIENT_VERTEX_ARRAY_BIT
    client_vertex_array_bit = GL_CLIENT_VERTEX_ARRAY_BIT,
#endif
}; // enum class client_attrib_mask
C_FLAGS(client_attrib_mask, ::libc_types::u32);

} // namespace gl::group
