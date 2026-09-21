#pragma once

#include "common.h"

namespace gl::group {

// TexStorageAttribs
enum class tex_storage_attribs : u32
{
    surface_compression_ext = 0x96C0, // GL_SURFACE_COMPRESSION_EXT
    surface_compression_fixed_rate_10bpc_ext =
        0x96CD, // GL_SURFACE_COMPRESSION_FIXED_RATE_10BPC_EXT
    surface_compression_fixed_rate_11bpc_ext =
        0x96CE, // GL_SURFACE_COMPRESSION_FIXED_RATE_11BPC_EXT
    surface_compression_fixed_rate_12bpc_ext =
        0x96CF, // GL_SURFACE_COMPRESSION_FIXED_RATE_12BPC_EXT
    surface_compression_fixed_rate_1bpc_ext =
        0x96C4, // GL_SURFACE_COMPRESSION_FIXED_RATE_1BPC_EXT
    surface_compression_fixed_rate_2bpc_ext =
        0x96C5, // GL_SURFACE_COMPRESSION_FIXED_RATE_2BPC_EXT
    surface_compression_fixed_rate_3bpc_ext =
        0x96C6, // GL_SURFACE_COMPRESSION_FIXED_RATE_3BPC_EXT
    surface_compression_fixed_rate_4bpc_ext =
        0x96C7, // GL_SURFACE_COMPRESSION_FIXED_RATE_4BPC_EXT
    surface_compression_fixed_rate_5bpc_ext =
        0x96C8, // GL_SURFACE_COMPRESSION_FIXED_RATE_5BPC_EXT
    surface_compression_fixed_rate_6bpc_ext =
        0x96C9, // GL_SURFACE_COMPRESSION_FIXED_RATE_6BPC_EXT
    surface_compression_fixed_rate_7bpc_ext =
        0x96CA, // GL_SURFACE_COMPRESSION_FIXED_RATE_7BPC_EXT
    surface_compression_fixed_rate_8bpc_ext =
        0x96CB, // GL_SURFACE_COMPRESSION_FIXED_RATE_8BPC_EXT
    surface_compression_fixed_rate_9bpc_ext =
        0x96CC, // GL_SURFACE_COMPRESSION_FIXED_RATE_9BPC_EXT
    surface_compression_fixed_rate_default_ext =
        0x96C2, // GL_SURFACE_COMPRESSION_FIXED_RATE_DEFAULT_EXT
    surface_compression_fixed_rate_none_ext =
        0x96C1, // GL_SURFACE_COMPRESSION_FIXED_RATE_NONE_EXT
}; // enum class tex_storage_attribs

} // namespace gl::group
