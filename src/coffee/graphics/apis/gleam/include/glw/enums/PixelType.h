#pragma once

#include "common.h"

namespace gl::group {

// PixelType
enum class pixel_type : u32
{
#ifdef GL_BITMAP
    bitmap = GL_BITMAP,
#endif
#ifdef GL_BYTE
    byte_ = GL_BYTE,
#endif
#ifdef GL_FLOAT
    float_ = GL_FLOAT,
#endif
#ifdef GL_FLOAT_32_UNSIGNED_INT_24_8_REV
    float_32_unsigned_int_24_8_rev = GL_FLOAT_32_UNSIGNED_INT_24_8_REV,
#endif
#ifdef GL_FLOAT_32_UNSIGNED_INT_24_8_REV_NV
    float_32_unsigned_int_24_8_rev_nv = GL_FLOAT_32_UNSIGNED_INT_24_8_REV_NV,
#endif
#ifdef GL_HALF_APPLE
    half_apple = GL_HALF_APPLE,
#endif
#ifdef GL_HALF_FLOAT
    half_float = GL_HALF_FLOAT,
#endif
#ifdef GL_HALF_FLOAT_ARB
    half_float_arb = GL_HALF_FLOAT_ARB,
#endif
#ifdef GL_HALF_FLOAT_NV
    half_float_nv = GL_HALF_FLOAT_NV,
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
#ifdef GL_UNSIGNED_BYTE_2_3_3_REV
    unsigned_byte_2_3_3_rev = GL_UNSIGNED_BYTE_2_3_3_REV,
#endif
#ifdef GL_UNSIGNED_BYTE_2_3_3_REV_EXT
    unsigned_byte_2_3_3_rev_ext = GL_UNSIGNED_BYTE_2_3_3_REV_EXT,
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
#ifdef GL_UNSIGNED_INT_10F_11F_11F_REV
    unsigned_int_10f_11f_11f_rev = GL_UNSIGNED_INT_10F_11F_11F_REV,
#endif
#ifdef GL_UNSIGNED_INT_10F_11F_11F_REV_APPLE
    unsigned_int_10f_11f_11f_rev_apple = GL_UNSIGNED_INT_10F_11F_11F_REV_APPLE,
#endif
#ifdef GL_UNSIGNED_INT_10F_11F_11F_REV_EXT
    unsigned_int_10f_11f_11f_rev_ext = GL_UNSIGNED_INT_10F_11F_11F_REV_EXT,
#endif
#ifdef GL_UNSIGNED_INT_10_10_10_2
    unsigned_int_10_10_10_2 = GL_UNSIGNED_INT_10_10_10_2,
#endif
#ifdef GL_UNSIGNED_INT_10_10_10_2_EXT
    unsigned_int_10_10_10_2_ext = GL_UNSIGNED_INT_10_10_10_2_EXT,
#endif
#ifdef GL_UNSIGNED_INT_24_8
    unsigned_int_24_8 = GL_UNSIGNED_INT_24_8,
#endif
#ifdef GL_UNSIGNED_INT_24_8_EXT
    unsigned_int_24_8_ext = GL_UNSIGNED_INT_24_8_EXT,
#endif
#ifdef GL_UNSIGNED_INT_24_8_NV
    unsigned_int_24_8_nv = GL_UNSIGNED_INT_24_8_NV,
#endif
#ifdef GL_UNSIGNED_INT_24_8_OES
    unsigned_int_24_8_oes = GL_UNSIGNED_INT_24_8_OES,
#endif
#ifdef GL_UNSIGNED_INT_2_10_10_10_REV
    unsigned_int_2_10_10_10_rev = GL_UNSIGNED_INT_2_10_10_10_REV,
#endif
#ifdef GL_UNSIGNED_INT_2_10_10_10_REV_EXT
    unsigned_int_2_10_10_10_rev_ext = GL_UNSIGNED_INT_2_10_10_10_REV_EXT,
#endif
#ifdef GL_UNSIGNED_INT_5_9_9_9_REV
    unsigned_int_5_9_9_9_rev = GL_UNSIGNED_INT_5_9_9_9_REV,
#endif
#ifdef GL_UNSIGNED_INT_5_9_9_9_REV_APPLE
    unsigned_int_5_9_9_9_rev_apple = GL_UNSIGNED_INT_5_9_9_9_REV_APPLE,
#endif
#ifdef GL_UNSIGNED_INT_5_9_9_9_REV_EXT
    unsigned_int_5_9_9_9_rev_ext = GL_UNSIGNED_INT_5_9_9_9_REV_EXT,
#endif
#ifdef GL_UNSIGNED_INT_8_8_8_8
    unsigned_int_8_8_8_8 = GL_UNSIGNED_INT_8_8_8_8,
#endif
#ifdef GL_UNSIGNED_INT_8_8_8_8_EXT
    unsigned_int_8_8_8_8_ext = GL_UNSIGNED_INT_8_8_8_8_EXT,
#endif
#ifdef GL_UNSIGNED_INT_8_8_8_8_REV
    unsigned_int_8_8_8_8_rev = GL_UNSIGNED_INT_8_8_8_8_REV,
#endif
#ifdef GL_UNSIGNED_INT_8_8_8_8_REV_EXT
    unsigned_int_8_8_8_8_rev_ext = GL_UNSIGNED_INT_8_8_8_8_REV_EXT,
#endif
#ifdef GL_UNSIGNED_SHORT
    unsigned_short = GL_UNSIGNED_SHORT,
#endif
#ifdef GL_UNSIGNED_SHORT_1_5_5_5_REV
    unsigned_short_1_5_5_5_rev = GL_UNSIGNED_SHORT_1_5_5_5_REV,
#endif
#ifdef GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT
    unsigned_short_1_5_5_5_rev_ext = GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT,
#endif
#ifdef GL_UNSIGNED_SHORT_4_4_4_4
    unsigned_short_4_4_4_4 = GL_UNSIGNED_SHORT_4_4_4_4,
#endif
#ifdef GL_UNSIGNED_SHORT_4_4_4_4_EXT
    unsigned_short_4_4_4_4_ext = GL_UNSIGNED_SHORT_4_4_4_4_EXT,
#endif
#ifdef GL_UNSIGNED_SHORT_4_4_4_4_REV
    unsigned_short_4_4_4_4_rev = GL_UNSIGNED_SHORT_4_4_4_4_REV,
#endif
#ifdef GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT
    unsigned_short_4_4_4_4_rev_ext = GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT,
#endif
#ifdef GL_UNSIGNED_SHORT_4_4_4_4_REV_IMG
    unsigned_short_4_4_4_4_rev_img = GL_UNSIGNED_SHORT_4_4_4_4_REV_IMG,
#endif
#ifdef GL_UNSIGNED_SHORT_5_5_5_1
    unsigned_short_5_5_5_1 = GL_UNSIGNED_SHORT_5_5_5_1,
#endif
#ifdef GL_UNSIGNED_SHORT_5_5_5_1_EXT
    unsigned_short_5_5_5_1_ext = GL_UNSIGNED_SHORT_5_5_5_1_EXT,
#endif
#ifdef GL_UNSIGNED_SHORT_5_6_5
    unsigned_short_5_6_5 = GL_UNSIGNED_SHORT_5_6_5,
#endif
#ifdef GL_UNSIGNED_SHORT_5_6_5_EXT
    unsigned_short_5_6_5_ext = GL_UNSIGNED_SHORT_5_6_5_EXT,
#endif
#ifdef GL_UNSIGNED_SHORT_5_6_5_REV
    unsigned_short_5_6_5_rev = GL_UNSIGNED_SHORT_5_6_5_REV,
#endif
#ifdef GL_UNSIGNED_SHORT_5_6_5_REV_EXT
    unsigned_short_5_6_5_rev_ext = GL_UNSIGNED_SHORT_5_6_5_REV_EXT,
#endif
}; // enum class pixel_type

} // namespace gl::group
