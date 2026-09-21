#pragma once

#include "common.h"

namespace gl::group {

// ErrorCode
enum class error_code : u32
{
    invalid_enum                  = 0x0500, // GL_INVALID_ENUM
    invalid_framebuffer_operation = 0x0506, // GL_INVALID_FRAMEBUFFER_OPERATION
    invalid_operation             = 0x0502, // GL_INVALID_OPERATION
    invalid_value                 = 0x0501, // GL_INVALID_VALUE
    no_error                      = 0,      // GL_NO_ERROR
    out_of_memory                 = 0x0505, // GL_OUT_OF_MEMORY
    stack_overflow                = 0x0503, // GL_STACK_OVERFLOW
    stack_underflow               = 0x0504, // GL_STACK_UNDERFLOW
    table_too_large               = 0x8031, // GL_TABLE_TOO_LARGE
    invalid_framebuffer_operation_ext =
        0x0506, // GL_INVALID_FRAMEBUFFER_OPERATION_EXT
    invalid_framebuffer_operation_oes =
        0x0506,                     // GL_INVALID_FRAMEBUFFER_OPERATION_OES
    table_too_large_ext   = 0x8031, // GL_TABLE_TOO_LARGE_EXT
    texture_too_large_ext = 0x8065, // GL_TEXTURE_TOO_LARGE_EXT
}; // enum class error_code

} // namespace gl::group
