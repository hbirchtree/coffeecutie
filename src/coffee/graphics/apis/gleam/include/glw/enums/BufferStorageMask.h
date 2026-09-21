#pragma once

#include "common.h"

namespace gl::group {

// BufferStorageMask
enum class buffer_storage_mask : u32
{
    client_storage_bit            = 0x0200, // GL_CLIENT_STORAGE_BIT
    dynamic_storage_bit           = 0x0100, // GL_DYNAMIC_STORAGE_BIT
    map_coherent_bit              = 0x0080, // GL_MAP_COHERENT_BIT
    map_persistent_bit            = 0x0040, // GL_MAP_PERSISTENT_BIT
    map_read_bit                  = 0x0001, // GL_MAP_READ_BIT
    map_write_bit                 = 0x0002, // GL_MAP_WRITE_BIT
    client_storage_bit_ext        = 0x0200, // GL_CLIENT_STORAGE_BIT_EXT
    dynamic_storage_bit_ext       = 0x0100, // GL_DYNAMIC_STORAGE_BIT_EXT
    external_storage_bit_nvx      = 0x2000, // GL_EXTERNAL_STORAGE_BIT_NVX
    lgpu_separate_storage_bit_nvx = 0x0800, // GL_LGPU_SEPARATE_STORAGE_BIT_NVX
    map_coherent_bit_ext          = 0x0080, // GL_MAP_COHERENT_BIT_EXT
    map_persistent_bit_ext        = 0x0040, // GL_MAP_PERSISTENT_BIT_EXT
    map_read_bit_ext              = 0x0001, // GL_MAP_READ_BIT_EXT
    map_write_bit_ext             = 0x0002, // GL_MAP_WRITE_BIT_EXT
    per_gpu_storage_bit_nv        = 0x0800, // GL_PER_GPU_STORAGE_BIT_NV
    sparse_storage_bit_arb        = 0x0400, // GL_SPARSE_STORAGE_BIT_ARB
}; // enum class buffer_storage_mask
C_FLAGS(buffer_storage_mask, u32);

} // namespace gl::group
