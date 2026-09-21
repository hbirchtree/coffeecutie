#pragma once

#include "common.h"

namespace gl::group {

// PixelType
enum class pixel_type : u32
{
    bitmap = 0x1A00, // GL_BITMAP
    byte_  = 0x1400, // GL_BYTE
    float_ = 0x1406, // GL_FLOAT
    float_32_unsigned_int_24_8_rev =
        0x8DAD,                            // GL_FLOAT_32_UNSIGNED_INT_24_8_REV
    half_float                   = 0x140B, // GL_HALF_FLOAT
    int_                         = 0x1404, // GL_INT
    short_                       = 0x1402, // GL_SHORT
    unsigned_byte                = 0x1401, // GL_UNSIGNED_BYTE
    unsigned_byte_2_3_3_rev      = 0x8362, // GL_UNSIGNED_BYTE_2_3_3_REV
    unsigned_byte_3_3_2          = 0x8032, // GL_UNSIGNED_BYTE_3_3_2
    unsigned_int                 = 0x1405, // GL_UNSIGNED_INT
    unsigned_int_10f_11f_11f_rev = 0x8C3B, // GL_UNSIGNED_INT_10F_11F_11F_REV
    unsigned_int_10_10_10_2      = 0x8036, // GL_UNSIGNED_INT_10_10_10_2
    unsigned_int_24_8            = 0x84FA, // GL_UNSIGNED_INT_24_8
    unsigned_int_2_10_10_10_rev  = 0x8368, // GL_UNSIGNED_INT_2_10_10_10_REV
    unsigned_int_5_9_9_9_rev     = 0x8C3E, // GL_UNSIGNED_INT_5_9_9_9_REV
    unsigned_int_8_8_8_8         = 0x8035, // GL_UNSIGNED_INT_8_8_8_8
    unsigned_int_8_8_8_8_rev     = 0x8367, // GL_UNSIGNED_INT_8_8_8_8_REV
    unsigned_short               = 0x1403, // GL_UNSIGNED_SHORT
    unsigned_short_1_5_5_5_rev   = 0x8366, // GL_UNSIGNED_SHORT_1_5_5_5_REV
    unsigned_short_4_4_4_4       = 0x8033, // GL_UNSIGNED_SHORT_4_4_4_4
    unsigned_short_4_4_4_4_rev   = 0x8365, // GL_UNSIGNED_SHORT_4_4_4_4_REV
    unsigned_short_5_5_5_1       = 0x8034, // GL_UNSIGNED_SHORT_5_5_5_1
    unsigned_short_5_6_5         = 0x8363, // GL_UNSIGNED_SHORT_5_6_5
    unsigned_short_5_6_5_rev     = 0x8364, // GL_UNSIGNED_SHORT_5_6_5_REV
    float_32_unsigned_int_24_8_rev_nv =
        0x8DAD, // GL_FLOAT_32_UNSIGNED_INT_24_8_REV_NV
    half_apple                  = 0x140B, // GL_HALF_APPLE
    half_float_arb              = 0x140B, // GL_HALF_FLOAT_ARB
    half_float_nv               = 0x140B, // GL_HALF_FLOAT_NV
    unsigned_byte_2_3_3_rev_ext = 0x8362, // GL_UNSIGNED_BYTE_2_3_3_REV_EXT
    unsigned_byte_3_3_2_ext     = 0x8032, // GL_UNSIGNED_BYTE_3_3_2_EXT
    unsigned_int_10f_11f_11f_rev_apple =
        0x8C3B, // GL_UNSIGNED_INT_10F_11F_11F_REV_APPLE
    unsigned_int_10f_11f_11f_rev_ext =
        0x8C3B,                           // GL_UNSIGNED_INT_10F_11F_11F_REV_EXT
    unsigned_int_10_10_10_2_ext = 0x8036, // GL_UNSIGNED_INT_10_10_10_2_EXT
    unsigned_int_24_8_ext       = 0x84FA, // GL_UNSIGNED_INT_24_8_EXT
    unsigned_int_24_8_nv        = 0x84FA, // GL_UNSIGNED_INT_24_8_NV
    unsigned_int_24_8_oes       = 0x84FA, // GL_UNSIGNED_INT_24_8_OES
    unsigned_int_2_10_10_10_rev_ext =
        0x8368, // GL_UNSIGNED_INT_2_10_10_10_REV_EXT
    unsigned_int_5_9_9_9_rev_apple =
        0x8C3E,                            // GL_UNSIGNED_INT_5_9_9_9_REV_APPLE
    unsigned_int_5_9_9_9_rev_ext = 0x8C3E, // GL_UNSIGNED_INT_5_9_9_9_REV_EXT
    unsigned_int_8_8_8_8_ext     = 0x8035, // GL_UNSIGNED_INT_8_8_8_8_EXT
    unsigned_int_8_8_8_8_rev_ext = 0x8367, // GL_UNSIGNED_INT_8_8_8_8_REV_EXT
    unsigned_short_1_5_5_5_rev_ext =
        0x8366,                          // GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT
    unsigned_short_4_4_4_4_ext = 0x8033, // GL_UNSIGNED_SHORT_4_4_4_4_EXT
    unsigned_short_4_4_4_4_rev_ext =
        0x8365, // GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT
    unsigned_short_4_4_4_4_rev_img =
        0x8365,                            // GL_UNSIGNED_SHORT_4_4_4_4_REV_IMG
    unsigned_short_5_5_5_1_ext   = 0x8034, // GL_UNSIGNED_SHORT_5_5_5_1_EXT
    unsigned_short_5_6_5_ext     = 0x8363, // GL_UNSIGNED_SHORT_5_6_5_EXT
    unsigned_short_5_6_5_rev_ext = 0x8364, // GL_UNSIGNED_SHORT_5_6_5_REV_EXT
}; // enum class pixel_type

} // namespace gl::group
