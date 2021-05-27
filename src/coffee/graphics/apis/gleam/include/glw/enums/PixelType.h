#pragma once

#include "common.h"

namespace gl::groups {

// PixelType
enum class pixel_type : ::libc_types::u32 {
#ifdef GL_BITMAP
    bitmap = GL_BITMAP,
#endif
#ifdef GL_BYTE
    byte_ = GL_BYTE,
#endif
#ifdef GL_FLOAT
    float_ = GL_FLOAT,
#endif
#ifdef GL_INT
    int_ = GL_INT,
#endif
#ifdef GL_SHORT
    short_ = GL_SHORT,
#endif
#ifdef GL_UNSIGNED_BYTE
    unsigned_byte = GL_UNSIGNED_BYTE,
#endif
#ifdef GL_UNSIGNED_BYTE_3_3_2
    unsigned_byte_3_3_2 = GL_UNSIGNED_BYTE_3_3_2,
#endif
#ifdef GL_UNSIGNED_BYTE_3_3_2_EXT
    unsigned_byte_3_3_2_ext = GL_UNSIGNED_BYTE_3_3_2_EXT,
#endif
#ifdef GL_UNSIGNED_INT
    unsigned_int = GL_UNSIGNED_INT,
#endif
#ifdef GL_UNSIGNED_INT_10_10_10_2
    unsigned_int_10_10_10_2 = GL_UNSIGNED_INT_10_10_10_2,
#endif
#ifdef GL_UNSIGNED_INT_10_10_10_2_EXT
    unsigned_int_10_10_10_2_ext = GL_UNSIGNED_INT_10_10_10_2_EXT,
#endif
#ifdef GL_UNSIGNED_INT_8_8_8_8
    unsigned_int_8_8_8_8 = GL_UNSIGNED_INT_8_8_8_8,
#endif
#ifdef GL_UNSIGNED_INT_8_8_8_8_EXT
    unsigned_int_8_8_8_8_ext = GL_UNSIGNED_INT_8_8_8_8_EXT,
#endif
#ifdef GL_UNSIGNED_SHORT
    unsigned_short = GL_UNSIGNED_SHORT,
#endif
#ifdef GL_UNSIGNED_SHORT_4_4_4_4
    unsigned_short_4_4_4_4 = GL_UNSIGNED_SHORT_4_4_4_4,
#endif
#ifdef GL_UNSIGNED_SHORT_4_4_4_4_EXT
    unsigned_short_4_4_4_4_ext = GL_UNSIGNED_SHORT_4_4_4_4_EXT,
#endif
#ifdef GL_UNSIGNED_SHORT_5_5_5_1
    unsigned_short_5_5_5_1 = GL_UNSIGNED_SHORT_5_5_5_1,
#endif
#ifdef GL_UNSIGNED_SHORT_5_5_5_1_EXT
    unsigned_short_5_5_5_1_ext = GL_UNSIGNED_SHORT_5_5_5_1_EXT,
#endif
}; // enum class pixel_type

} // namespace gl::groups
