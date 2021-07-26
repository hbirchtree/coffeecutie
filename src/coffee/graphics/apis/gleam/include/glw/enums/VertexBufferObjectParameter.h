#pragma once

#include "common.h"

namespace gl::group {

// VertexBufferObjectParameter
enum class vertex_buffer_object_parameter : ::libc_types::u32
{
#ifdef GL_BUFFER_ACCESS
    buffer_access = GL_BUFFER_ACCESS,
#endif
#ifdef GL_BUFFER_ACCESS_FLAGS
    buffer_access_flags = GL_BUFFER_ACCESS_FLAGS,
#endif
#ifdef GL_BUFFER_IMMUTABLE_STORAGE
    buffer_immutable_storage = GL_BUFFER_IMMUTABLE_STORAGE,
#endif
#ifdef GL_BUFFER_MAPPED
    buffer_mapped = GL_BUFFER_MAPPED,
#endif
#ifdef GL_BUFFER_MAP_LENGTH
    buffer_map_length = GL_BUFFER_MAP_LENGTH,
#endif
#ifdef GL_BUFFER_MAP_OFFSET
    buffer_map_offset = GL_BUFFER_MAP_OFFSET,
#endif
#ifdef GL_BUFFER_SIZE
    buffer_size = GL_BUFFER_SIZE,
#endif
#ifdef GL_BUFFER_STORAGE_FLAGS
    buffer_storage_flags = GL_BUFFER_STORAGE_FLAGS,
#endif
#ifdef GL_BUFFER_USAGE
    buffer_usage = GL_BUFFER_USAGE,
#endif
}; // enum class vertex_buffer_object_parameter

} // namespace gl::group
