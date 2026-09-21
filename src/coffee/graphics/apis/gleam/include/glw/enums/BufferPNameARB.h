#pragma once

#include "common.h"

namespace gl::group {

// BufferPNameARB
enum class buffer_prop_arb : u32
{
    buffer_access            = 0x88BB, // GL_BUFFER_ACCESS
    buffer_access_flags      = 0x911F, // GL_BUFFER_ACCESS_FLAGS
    buffer_immutable_storage = 0x821F, // GL_BUFFER_IMMUTABLE_STORAGE
    buffer_mapped            = 0x88BC, // GL_BUFFER_MAPPED
    buffer_map_length        = 0x9120, // GL_BUFFER_MAP_LENGTH
    buffer_map_offset        = 0x9121, // GL_BUFFER_MAP_OFFSET
    buffer_size              = 0x8764, // GL_BUFFER_SIZE
    buffer_storage_flags     = 0x8220, // GL_BUFFER_STORAGE_FLAGS
    buffer_usage             = 0x8765, // GL_BUFFER_USAGE
    buffer_access_arb        = 0x88BB, // GL_BUFFER_ACCESS_ARB
    buffer_client_pointer_size_mesa =
        0x9790,                 // GL_BUFFER_CLIENT_POINTER_SIZE_MESA
    buffer_mapped_arb = 0x88BC, // GL_BUFFER_MAPPED_ARB
    buffer_size_arb   = 0x8764, // GL_BUFFER_SIZE_ARB
    buffer_usage_arb  = 0x8765, // GL_BUFFER_USAGE_ARB
}; // enum class buffer_prop_arb

} // namespace gl::group
