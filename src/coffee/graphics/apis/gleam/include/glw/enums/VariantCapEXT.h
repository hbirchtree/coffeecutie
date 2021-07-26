#pragma once

#include "common.h"

namespace gl::group {

// VariantCapEXT
enum class variant_cap_ext : ::libc_types::u32
{
#ifdef GL_VARIANT_ARRAY_EXT
    variant_array_ext = GL_VARIANT_ARRAY_EXT,
#endif
}; // enum class variant_cap_ext

} // namespace gl::group
