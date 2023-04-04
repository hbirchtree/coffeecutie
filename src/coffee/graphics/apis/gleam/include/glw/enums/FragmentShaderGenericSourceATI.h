#pragma once

#include "common.h"

namespace gl::group {

// FragmentShaderGenericSourceATI
enum class fragment_shader_generic_source_ati : u32
{
#ifdef GL_CON_0_ATI
    con_0_ati = GL_CON_0_ATI,
#endif
#ifdef GL_CON_10_ATI
    con_10_ati = GL_CON_10_ATI,
#endif
#ifdef GL_CON_11_ATI
    con_11_ati = GL_CON_11_ATI,
#endif
#ifdef GL_CON_12_ATI
    con_12_ati = GL_CON_12_ATI,
#endif
#ifdef GL_CON_13_ATI
    con_13_ati = GL_CON_13_ATI,
#endif
#ifdef GL_CON_14_ATI
    con_14_ati = GL_CON_14_ATI,
#endif
#ifdef GL_CON_15_ATI
    con_15_ati = GL_CON_15_ATI,
#endif
#ifdef GL_CON_16_ATI
    con_16_ati = GL_CON_16_ATI,
#endif
#ifdef GL_CON_17_ATI
    con_17_ati = GL_CON_17_ATI,
#endif
#ifdef GL_CON_18_ATI
    con_18_ati = GL_CON_18_ATI,
#endif
#ifdef GL_CON_19_ATI
    con_19_ati = GL_CON_19_ATI,
#endif
#ifdef GL_CON_1_ATI
    con_1_ati = GL_CON_1_ATI,
#endif
#ifdef GL_CON_20_ATI
    con_20_ati = GL_CON_20_ATI,
#endif
#ifdef GL_CON_21_ATI
    con_21_ati = GL_CON_21_ATI,
#endif
#ifdef GL_CON_22_ATI
    con_22_ati = GL_CON_22_ATI,
#endif
#ifdef GL_CON_23_ATI
    con_23_ati = GL_CON_23_ATI,
#endif
#ifdef GL_CON_24_ATI
    con_24_ati = GL_CON_24_ATI,
#endif
#ifdef GL_CON_25_ATI
    con_25_ati = GL_CON_25_ATI,
#endif
#ifdef GL_CON_26_ATI
    con_26_ati = GL_CON_26_ATI,
#endif
#ifdef GL_CON_27_ATI
    con_27_ati = GL_CON_27_ATI,
#endif
#ifdef GL_CON_28_ATI
    con_28_ati = GL_CON_28_ATI,
#endif
#ifdef GL_CON_29_ATI
    con_29_ati = GL_CON_29_ATI,
#endif
#ifdef GL_CON_2_ATI
    con_2_ati = GL_CON_2_ATI,
#endif
#ifdef GL_CON_30_ATI
    con_30_ati = GL_CON_30_ATI,
#endif
#ifdef GL_CON_31_ATI
    con_31_ati = GL_CON_31_ATI,
#endif
#ifdef GL_CON_3_ATI
    con_3_ati = GL_CON_3_ATI,
#endif
#ifdef GL_CON_4_ATI
    con_4_ati = GL_CON_4_ATI,
#endif
#ifdef GL_CON_5_ATI
    con_5_ati = GL_CON_5_ATI,
#endif
#ifdef GL_CON_6_ATI
    con_6_ati = GL_CON_6_ATI,
#endif
#ifdef GL_CON_7_ATI
    con_7_ati = GL_CON_7_ATI,
#endif
#ifdef GL_CON_8_ATI
    con_8_ati = GL_CON_8_ATI,
#endif
#ifdef GL_CON_9_ATI
    con_9_ati = GL_CON_9_ATI,
#endif
#ifdef GL_ONE
    one = GL_ONE,
#endif
#ifdef GL_PRIMARY_COLOR
    primary_color = GL_PRIMARY_COLOR,
#endif
#ifdef GL_PRIMARY_COLOR_ARB
    primary_color_arb = GL_PRIMARY_COLOR_ARB,
#endif
#ifdef GL_PRIMARY_COLOR_EXT
    primary_color_ext = GL_PRIMARY_COLOR_EXT,
#endif
#ifdef GL_REG_0_ATI
    reg_0_ati = GL_REG_0_ATI,
#endif
#ifdef GL_REG_10_ATI
    reg_10_ati = GL_REG_10_ATI,
#endif
#ifdef GL_REG_11_ATI
    reg_11_ati = GL_REG_11_ATI,
#endif
#ifdef GL_REG_12_ATI
    reg_12_ati = GL_REG_12_ATI,
#endif
#ifdef GL_REG_13_ATI
    reg_13_ati = GL_REG_13_ATI,
#endif
#ifdef GL_REG_14_ATI
    reg_14_ati = GL_REG_14_ATI,
#endif
#ifdef GL_REG_15_ATI
    reg_15_ati = GL_REG_15_ATI,
#endif
#ifdef GL_REG_16_ATI
    reg_16_ati = GL_REG_16_ATI,
#endif
#ifdef GL_REG_17_ATI
    reg_17_ati = GL_REG_17_ATI,
#endif
#ifdef GL_REG_18_ATI
    reg_18_ati = GL_REG_18_ATI,
#endif
#ifdef GL_REG_19_ATI
    reg_19_ati = GL_REG_19_ATI,
#endif
#ifdef GL_REG_1_ATI
    reg_1_ati = GL_REG_1_ATI,
#endif
#ifdef GL_REG_20_ATI
    reg_20_ati = GL_REG_20_ATI,
#endif
#ifdef GL_REG_21_ATI
    reg_21_ati = GL_REG_21_ATI,
#endif
#ifdef GL_REG_22_ATI
    reg_22_ati = GL_REG_22_ATI,
#endif
#ifdef GL_REG_23_ATI
    reg_23_ati = GL_REG_23_ATI,
#endif
#ifdef GL_REG_24_ATI
    reg_24_ati = GL_REG_24_ATI,
#endif
#ifdef GL_REG_25_ATI
    reg_25_ati = GL_REG_25_ATI,
#endif
#ifdef GL_REG_26_ATI
    reg_26_ati = GL_REG_26_ATI,
#endif
#ifdef GL_REG_27_ATI
    reg_27_ati = GL_REG_27_ATI,
#endif
#ifdef GL_REG_28_ATI
    reg_28_ati = GL_REG_28_ATI,
#endif
#ifdef GL_REG_29_ATI
    reg_29_ati = GL_REG_29_ATI,
#endif
#ifdef GL_REG_2_ATI
    reg_2_ati = GL_REG_2_ATI,
#endif
#ifdef GL_REG_30_ATI
    reg_30_ati = GL_REG_30_ATI,
#endif
#ifdef GL_REG_31_ATI
    reg_31_ati = GL_REG_31_ATI,
#endif
#ifdef GL_REG_3_ATI
    reg_3_ati = GL_REG_3_ATI,
#endif
#ifdef GL_REG_4_ATI
    reg_4_ati = GL_REG_4_ATI,
#endif
#ifdef GL_REG_5_ATI
    reg_5_ati = GL_REG_5_ATI,
#endif
#ifdef GL_REG_6_ATI
    reg_6_ati = GL_REG_6_ATI,
#endif
#ifdef GL_REG_7_ATI
    reg_7_ati = GL_REG_7_ATI,
#endif
#ifdef GL_REG_8_ATI
    reg_8_ati = GL_REG_8_ATI,
#endif
#ifdef GL_REG_9_ATI
    reg_9_ati = GL_REG_9_ATI,
#endif
#ifdef GL_SECONDARY_INTERPOLATOR_ATI
    secondary_interpolator_ati = GL_SECONDARY_INTERPOLATOR_ATI,
#endif
#ifdef GL_ZERO
    zero = GL_ZERO,
#endif
}; // enum class fragment_shader_generic_source_ati

} // namespace gl::group
