#pragma once

#include "common.h"

namespace gl::groups {

// BufferPNameARB
enum class buffer_prop_arb : ::libc_types::u32 {
#ifdef GL_BUFFER_ACCESS
    buffer_access = GL_BUFFER_ACCESS,
#endif
#ifdef GL_BUFFER_ACCESS_ARB
    buffer_access_arb = GL_BUFFER_ACCESS_ARB,
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
#ifdef GL_BUFFER_MAPPED_ARB
    buffer_mapped_arb = GL_BUFFER_MAPPED_ARB,
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
#ifdef GL_BUFFER_SIZE_ARB
    buffer_size_arb = GL_BUFFER_SIZE_ARB,
#endif
#ifdef GL_BUFFER_STORAGE_FLAGS
    buffer_storage_flags = GL_BUFFER_STORAGE_FLAGS,
#endif
#ifdef GL_BUFFER_USAGE
    buffer_usage = GL_BUFFER_USAGE,
#endif
#ifdef GL_BUFFER_USAGE_ARB
    buffer_usage_arb = GL_BUFFER_USAGE_ARB,
#endif
}; // enum class buffer_prop_arb

} // namespace gl::groups
