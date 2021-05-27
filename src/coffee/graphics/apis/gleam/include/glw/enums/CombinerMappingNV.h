#pragma once

#include "common.h"

namespace gl::groups {

// CombinerMappingNV
enum class combiner_mapping_nv : ::libc_types::u32 {
#ifdef GL_EXPAND_NEGATE_NV
    expand_negate_nv = GL_EXPAND_NEGATE_NV,
#endif
#ifdef GL_EXPAND_NORMAL_NV
    expand_normal_nv = GL_EXPAND_NORMAL_NV,
#endif
#ifdef GL_HALF_BIAS_NEGATE_NV
    half_bias_negate_nv = GL_HALF_BIAS_NEGATE_NV,
#endif
#ifdef GL_HALF_BIAS_NORMAL_NV
    half_bias_normal_nv = GL_HALF_BIAS_NORMAL_NV,
#endif
#ifdef GL_SIGNED_IDENTITY_NV
    signed_identity_nv = GL_SIGNED_IDENTITY_NV,
#endif
#ifdef GL_SIGNED_NEGATE_NV
    signed_negate_nv = GL_SIGNED_NEGATE_NV,
#endif
#ifdef GL_UNSIGNED_IDENTITY_NV
    unsigned_identity_nv = GL_UNSIGNED_IDENTITY_NV,
#endif
#ifdef GL_UNSIGNED_INVERT_NV
    unsigned_invert_nv = GL_UNSIGNED_INVERT_NV,
#endif
}; // enum class combiner_mapping_nv

} // namespace gl::groups
