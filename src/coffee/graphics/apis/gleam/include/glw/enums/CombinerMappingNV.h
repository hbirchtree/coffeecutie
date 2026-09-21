#pragma once

#include "common.h"

namespace gl::group {

// CombinerMappingNV
enum class combiner_mapping_nv : u32
{
    expand_negate_nv     = 0x8539, // GL_EXPAND_NEGATE_NV
    expand_normal_nv     = 0x8538, // GL_EXPAND_NORMAL_NV
    half_bias_negate_nv  = 0x853B, // GL_HALF_BIAS_NEGATE_NV
    half_bias_normal_nv  = 0x853A, // GL_HALF_BIAS_NORMAL_NV
    signed_identity_nv   = 0x853C, // GL_SIGNED_IDENTITY_NV
    signed_negate_nv     = 0x853D, // GL_SIGNED_NEGATE_NV
    unsigned_identity_nv = 0x8536, // GL_UNSIGNED_IDENTITY_NV
    unsigned_invert_nv   = 0x8537, // GL_UNSIGNED_INVERT_NV
}; // enum class combiner_mapping_nv

} // namespace gl::group
