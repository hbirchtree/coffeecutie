#pragma once

#include "common.h"

namespace gl::group {

// ErrorCode
enum class error_code : u32
{
#ifdef GL_INVALID_ENUM
    invalid_enum = GL_INVALID_ENUM,
#endif
#ifdef GL_INVALID_FRAMEBUFFER_OPERATION
    invalid_framebuffer_operation = GL_INVALID_FRAMEBUFFER_OPERATION,
#endif
#ifdef GL_INVALID_FRAMEBUFFER_OPERATION_EXT
    invalid_framebuffer_operation_ext = GL_INVALID_FRAMEBUFFER_OPERATION_EXT,
#endif
#ifdef GL_INVALID_FRAMEBUFFER_OPERATION_OES
    invalid_framebuffer_operation_oes = GL_INVALID_FRAMEBUFFER_OPERATION_OES,
#endif
#ifdef GL_INVALID_OPERATION
    invalid_operation = GL_INVALID_OPERATION,
#endif
#ifdef GL_INVALID_VALUE
    invalid_value = GL_INVALID_VALUE,
#endif
#ifdef GL_NO_ERROR
    no_error = GL_NO_ERROR,
#endif
#ifdef GL_OUT_OF_MEMORY
    out_of_memory = GL_OUT_OF_MEMORY,
#endif
#ifdef GL_STACK_OVERFLOW
    stack_overflow = GL_STACK_OVERFLOW,
#endif
#ifdef GL_STACK_UNDERFLOW
    stack_underflow = GL_STACK_UNDERFLOW,
#endif
#ifdef GL_TABLE_TOO_LARGE
    table_too_large = GL_TABLE_TOO_LARGE,
#endif
#ifdef GL_TABLE_TOO_LARGE_EXT
    table_too_large_ext = GL_TABLE_TOO_LARGE_EXT,
#endif
#ifdef GL_TEXTURE_TOO_LARGE_EXT
    texture_too_large_ext = GL_TEXTURE_TOO_LARGE_EXT,
#endif
}; // enum class error_code

} // namespace gl::group
