#pragma once

#include "common.h"

namespace gl::group {

// MapBufferAccessMask
enum class map_buffer_access_mask : u32
{
#ifdef GL_MAP_COHERENT_BIT
    map_coherent_bit = GL_MAP_COHERENT_BIT,
#endif
#ifdef GL_MAP_COHERENT_BIT_EXT
    map_coherent_bit_ext = GL_MAP_COHERENT_BIT_EXT,
#endif
#ifdef GL_MAP_FLUSH_EXPLICIT_BIT
    map_flush_explicit_bit = GL_MAP_FLUSH_EXPLICIT_BIT,
#endif
#ifdef GL_MAP_FLUSH_EXPLICIT_BIT_EXT
    map_flush_explicit_bit_ext = GL_MAP_FLUSH_EXPLICIT_BIT_EXT,
#endif
#ifdef GL_MAP_INVALIDATE_BUFFER_BIT
    map_invalidate_buffer_bit = GL_MAP_INVALIDATE_BUFFER_BIT,
#endif
#ifdef GL_MAP_INVALIDATE_BUFFER_BIT_EXT
    map_invalidate_buffer_bit_ext = GL_MAP_INVALIDATE_BUFFER_BIT_EXT,
#endif
#ifdef GL_MAP_INVALIDATE_RANGE_BIT
    map_invalidate_range_bit = GL_MAP_INVALIDATE_RANGE_BIT,
#endif
#ifdef GL_MAP_INVALIDATE_RANGE_BIT_EXT
    map_invalidate_range_bit_ext = GL_MAP_INVALIDATE_RANGE_BIT_EXT,
#endif
#ifdef GL_MAP_PERSISTENT_BIT
    map_persistent_bit = GL_MAP_PERSISTENT_BIT,
#endif
#ifdef GL_MAP_PERSISTENT_BIT_EXT
    map_persistent_bit_ext = GL_MAP_PERSISTENT_BIT_EXT,
#endif
#ifdef GL_MAP_READ_BIT
    map_read_bit = GL_MAP_READ_BIT,
#endif
#ifdef GL_MAP_READ_BIT_EXT
    map_read_bit_ext = GL_MAP_READ_BIT_EXT,
#endif
#ifdef GL_MAP_UNSYNCHRONIZED_BIT
    map_unsynchronized_bit = GL_MAP_UNSYNCHRONIZED_BIT,
#endif
#ifdef GL_MAP_UNSYNCHRONIZED_BIT_EXT
    map_unsynchronized_bit_ext = GL_MAP_UNSYNCHRONIZED_BIT_EXT,
#endif
#ifdef GL_MAP_WRITE_BIT
    map_write_bit = GL_MAP_WRITE_BIT,
#endif
#ifdef GL_MAP_WRITE_BIT_EXT
    map_write_bit_ext = GL_MAP_WRITE_BIT_EXT,
#endif
}; // enum class map_buffer_access_mask
C_FLAGS(map_buffer_access_mask, u32);

} // namespace gl::group
