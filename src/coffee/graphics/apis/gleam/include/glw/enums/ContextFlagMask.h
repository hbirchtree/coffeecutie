#pragma once

#include "common.h"

namespace gl::group {

// ContextFlagMask
enum class context_flag_mask : ::libc_types::u32
{
#ifdef GL_CONTEXT_FLAG_DEBUG_BIT
    context_flag_debug_bit = GL_CONTEXT_FLAG_DEBUG_BIT,
#endif
#ifdef GL_CONTEXT_FLAG_DEBUG_BIT_KHR
    context_flag_debug_bit_khr = GL_CONTEXT_FLAG_DEBUG_BIT_KHR,
#endif
#ifdef GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT
    context_flag_forward_compatible_bit =
        GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT,
#endif
#ifdef GL_CONTEXT_FLAG_NO_ERROR_BIT
    context_flag_no_error_bit = GL_CONTEXT_FLAG_NO_ERROR_BIT,
#endif
#ifdef GL_CONTEXT_FLAG_NO_ERROR_BIT_KHR
    context_flag_no_error_bit_khr = GL_CONTEXT_FLAG_NO_ERROR_BIT_KHR,
#endif
#ifdef GL_CONTEXT_FLAG_PROTECTED_CONTENT_BIT_EXT
    context_flag_protected_content_bit_ext =
        GL_CONTEXT_FLAG_PROTECTED_CONTENT_BIT_EXT,
#endif
#ifdef GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT
    context_flag_robust_access_bit = GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT,
#endif
#ifdef GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT_ARB
    context_flag_robust_access_bit_arb = GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT_ARB,
#endif
}; // enum class context_flag_mask
C_FLAGS(context_flag_mask, ::libc_types::u32);

} // namespace gl::group
