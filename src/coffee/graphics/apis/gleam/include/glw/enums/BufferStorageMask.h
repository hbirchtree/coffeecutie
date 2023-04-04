#pragma once

#include "common.h"

namespace gl::group {

// BufferStorageMask
enum class buffer_storage_mask : u32
{
#ifdef GL_CLIENT_STORAGE_BIT
    client_storage_bit = GL_CLIENT_STORAGE_BIT,
#endif
#ifdef GL_CLIENT_STORAGE_BIT_EXT
    client_storage_bit_ext = GL_CLIENT_STORAGE_BIT_EXT,
#endif
#ifdef GL_DYNAMIC_STORAGE_BIT
    dynamic_storage_bit = GL_DYNAMIC_STORAGE_BIT,
#endif
#ifdef GL_DYNAMIC_STORAGE_BIT_EXT
    dynamic_storage_bit_ext = GL_DYNAMIC_STORAGE_BIT_EXT,
#endif
#ifdef GL_EXTERNAL_STORAGE_BIT_NVX
    external_storage_bit_nvx = GL_EXTERNAL_STORAGE_BIT_NVX,
#endif
#ifdef GL_LGPU_SEPARATE_STORAGE_BIT_NVX
    lgpu_separate_storage_bit_nvx = GL_LGPU_SEPARATE_STORAGE_BIT_NVX,
#endif
#ifdef GL_MAP_COHERENT_BIT
    map_coherent_bit = GL_MAP_COHERENT_BIT,
#endif
#ifdef GL_MAP_COHERENT_BIT_EXT
    map_coherent_bit_ext = GL_MAP_COHERENT_BIT_EXT,
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
#ifdef GL_MAP_WRITE_BIT
    map_write_bit = GL_MAP_WRITE_BIT,
#endif
#ifdef GL_MAP_WRITE_BIT_EXT
    map_write_bit_ext = GL_MAP_WRITE_BIT_EXT,
#endif
#ifdef GL_PER_GPU_STORAGE_BIT_NV
    per_gpu_storage_bit_nv = GL_PER_GPU_STORAGE_BIT_NV,
#endif
#ifdef GL_SPARSE_STORAGE_BIT_ARB
    sparse_storage_bit_arb = GL_SPARSE_STORAGE_BIT_ARB,
#endif
}; // enum class buffer_storage_mask
C_FLAGS(buffer_storage_mask, ::libc_types::u32);

} // namespace gl::group
