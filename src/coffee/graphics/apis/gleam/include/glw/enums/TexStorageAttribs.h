#pragma once

#include "common.h"

namespace gl::group {

// TexStorageAttribs
enum class tex_storage_attribs : ::libc_types::u32
{
#ifdef GL_SURFACE_COMPRESSION_EXT
    surface_compression_ext = GL_SURFACE_COMPRESSION_EXT,
#endif
#ifdef GL_SURFACE_COMPRESSION_FIXED_RATE_10BPC_EXT
    surface_compression_fixed_rate_10bpc_ext =
        GL_SURFACE_COMPRESSION_FIXED_RATE_10BPC_EXT,
#endif
#ifdef GL_SURFACE_COMPRESSION_FIXED_RATE_11BPC_EXT
    surface_compression_fixed_rate_11bpc_ext =
        GL_SURFACE_COMPRESSION_FIXED_RATE_11BPC_EXT,
#endif
#ifdef GL_SURFACE_COMPRESSION_FIXED_RATE_12BPC_EXT
    surface_compression_fixed_rate_12bpc_ext =
        GL_SURFACE_COMPRESSION_FIXED_RATE_12BPC_EXT,
#endif
#ifdef GL_SURFACE_COMPRESSION_FIXED_RATE_1BPC_EXT
    surface_compression_fixed_rate_1bpc_ext =
        GL_SURFACE_COMPRESSION_FIXED_RATE_1BPC_EXT,
#endif
#ifdef GL_SURFACE_COMPRESSION_FIXED_RATE_2BPC_EXT
    surface_compression_fixed_rate_2bpc_ext =
        GL_SURFACE_COMPRESSION_FIXED_RATE_2BPC_EXT,
#endif
#ifdef GL_SURFACE_COMPRESSION_FIXED_RATE_3BPC_EXT
    surface_compression_fixed_rate_3bpc_ext =
        GL_SURFACE_COMPRESSION_FIXED_RATE_3BPC_EXT,
#endif
#ifdef GL_SURFACE_COMPRESSION_FIXED_RATE_4BPC_EXT
    surface_compression_fixed_rate_4bpc_ext =
        GL_SURFACE_COMPRESSION_FIXED_RATE_4BPC_EXT,
#endif
#ifdef GL_SURFACE_COMPRESSION_FIXED_RATE_5BPC_EXT
    surface_compression_fixed_rate_5bpc_ext =
        GL_SURFACE_COMPRESSION_FIXED_RATE_5BPC_EXT,
#endif
#ifdef GL_SURFACE_COMPRESSION_FIXED_RATE_6BPC_EXT
    surface_compression_fixed_rate_6bpc_ext =
        GL_SURFACE_COMPRESSION_FIXED_RATE_6BPC_EXT,
#endif
#ifdef GL_SURFACE_COMPRESSION_FIXED_RATE_7BPC_EXT
    surface_compression_fixed_rate_7bpc_ext =
        GL_SURFACE_COMPRESSION_FIXED_RATE_7BPC_EXT,
#endif
#ifdef GL_SURFACE_COMPRESSION_FIXED_RATE_8BPC_EXT
    surface_compression_fixed_rate_8bpc_ext =
        GL_SURFACE_COMPRESSION_FIXED_RATE_8BPC_EXT,
#endif
#ifdef GL_SURFACE_COMPRESSION_FIXED_RATE_9BPC_EXT
    surface_compression_fixed_rate_9bpc_ext =
        GL_SURFACE_COMPRESSION_FIXED_RATE_9BPC_EXT,
#endif
#ifdef GL_SURFACE_COMPRESSION_FIXED_RATE_DEFAULT_EXT
    surface_compression_fixed_rate_default_ext =
        GL_SURFACE_COMPRESSION_FIXED_RATE_DEFAULT_EXT,
#endif
#ifdef GL_SURFACE_COMPRESSION_FIXED_RATE_NONE_EXT
    surface_compression_fixed_rate_none_ext =
        GL_SURFACE_COMPRESSION_FIXED_RATE_NONE_EXT,
#endif
}; // enum class tex_storage_attribs

} // namespace gl::group
