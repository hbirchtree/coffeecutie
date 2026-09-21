#pragma once

#include "common.h"

namespace gl::group {

// MapBufferAccessMask
enum class map_buffer_access_mask : u32
{
    map_coherent_bit              = 0x0080, // GL_MAP_COHERENT_BIT
    map_flush_explicit_bit        = 0x0010, // GL_MAP_FLUSH_EXPLICIT_BIT
    map_invalidate_buffer_bit     = 0x0008, // GL_MAP_INVALIDATE_BUFFER_BIT
    map_invalidate_range_bit      = 0x0004, // GL_MAP_INVALIDATE_RANGE_BIT
    map_persistent_bit            = 0x0040, // GL_MAP_PERSISTENT_BIT
    map_read_bit                  = 0x0001, // GL_MAP_READ_BIT
    map_unsynchronized_bit        = 0x0020, // GL_MAP_UNSYNCHRONIZED_BIT
    map_write_bit                 = 0x0002, // GL_MAP_WRITE_BIT
    map_client_pointer_bit_mesa   = 0x4000, // GL_MAP_CLIENT_POINTER_BIT_MESA
    map_coherent_bit_ext          = 0x0080, // GL_MAP_COHERENT_BIT_EXT
    map_flush_explicit_bit_ext    = 0x0010, // GL_MAP_FLUSH_EXPLICIT_BIT_EXT
    map_invalidate_buffer_bit_ext = 0x0008, // GL_MAP_INVALIDATE_BUFFER_BIT_EXT
    map_invalidate_range_bit_ext  = 0x0004, // GL_MAP_INVALIDATE_RANGE_BIT_EXT
    map_persistent_bit_ext        = 0x0040, // GL_MAP_PERSISTENT_BIT_EXT
    map_read_bit_ext              = 0x0001, // GL_MAP_READ_BIT_EXT
    map_unsynchronized_bit_ext    = 0x0020, // GL_MAP_UNSYNCHRONIZED_BIT_EXT
    map_write_bit_ext             = 0x0002, // GL_MAP_WRITE_BIT_EXT
}; // enum class map_buffer_access_mask
C_FLAGS(map_buffer_access_mask, u32);

} // namespace gl::group
