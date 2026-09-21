#pragma once

#include "common.h"

namespace gl::group {

// TextureStorageMaskAMD
enum class texture_storage_mask_amd : u32
{
    texture_storage_sparse_bit_amd =
        0x00000001, // GL_TEXTURE_STORAGE_SPARSE_BIT_AMD
}; // enum class texture_storage_mask_amd
C_FLAGS(texture_storage_mask_amd, u32);

} // namespace gl::group
