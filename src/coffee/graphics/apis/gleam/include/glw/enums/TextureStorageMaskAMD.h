#pragma once

#include "common.h"

namespace gl::groups {

// TextureStorageMaskAMD
enum class texture_storage_mask_amd : ::libc_types::u32 {
#ifdef GL_TEXTURE_STORAGE_SPARSE_BIT_AMD
    texture_storage_sparse_bit_amd = GL_TEXTURE_STORAGE_SPARSE_BIT_AMD,
#endif
}; // enum class texture_storage_mask_amd
C_FLAGS(texture_storage_mask_amd, ::libc_types::u32);

} // namespace gl::groups
