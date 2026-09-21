#pragma once

#include "common.h"

namespace gl::group {

// ContextFlagMask
enum class context_flag_mask : u32
{
    context_flag_debug_bit = 0x00000002, // GL_CONTEXT_FLAG_DEBUG_BIT
    context_flag_forward_compatible_bit =
        0x00000001, // GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT
    context_flag_no_error_bit = 0x00000008, // GL_CONTEXT_FLAG_NO_ERROR_BIT
    context_flag_robust_access_bit =
        0x00000004, // GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT
    context_flag_debug_bit_khr = 0x00000002, // GL_CONTEXT_FLAG_DEBUG_BIT_KHR
    context_flag_no_error_bit_khr =
        0x00000008, // GL_CONTEXT_FLAG_NO_ERROR_BIT_KHR
    context_flag_protected_content_bit_ext =
        0x00000010, // GL_CONTEXT_FLAG_PROTECTED_CONTENT_BIT_EXT
    context_flag_robust_access_bit_arb =
        0x00000004, // GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT_ARB
}; // enum class context_flag_mask
C_FLAGS(context_flag_mask, u32);

} // namespace gl::group
