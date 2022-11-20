#pragma once

#include "common.h"

namespace gl::group {

// TextureEnvParameter
enum class texture_env_parameter : ::libc_types::u32
{
#ifdef GL_ADD_SIGNED
    add_signed = GL_ADD_SIGNED,
#endif
#ifdef GL_ADD_SIGNED_ARB
    add_signed_arb = GL_ADD_SIGNED_ARB,
#endif
#ifdef GL_ADD_SIGNED_EXT
    add_signed_ext = GL_ADD_SIGNED_EXT,
#endif
#ifdef GL_COMBINE
    combine = GL_COMBINE,
#endif
#ifdef GL_COMBINE_ALPHA
    combine_alpha = GL_COMBINE_ALPHA,
#endif
#ifdef GL_COMBINE_ALPHA_ARB
    combine_alpha_arb = GL_COMBINE_ALPHA_ARB,
#endif
#ifdef GL_COMBINE_ALPHA_EXT
    combine_alpha_ext = GL_COMBINE_ALPHA_EXT,
#endif
#ifdef GL_COMBINE_ARB
    combine_arb = GL_COMBINE_ARB,
#endif
#ifdef GL_COMBINE_EXT
    combine_ext = GL_COMBINE_EXT,
#endif
#ifdef GL_COMBINE_RGB
    combine_rgb = GL_COMBINE_RGB,
#endif
#ifdef GL_COMBINE_RGB_ARB
    combine_rgb_arb = GL_COMBINE_RGB_ARB,
#endif
#ifdef GL_COMBINE_RGB_EXT
    combine_rgb_ext = GL_COMBINE_RGB_EXT,
#endif
#ifdef GL_CONSTANT
    constant = GL_CONSTANT,
#endif
#ifdef GL_CONSTANT_ARB
    constant_arb = GL_CONSTANT_ARB,
#endif
#ifdef GL_CONSTANT_EXT
    constant_ext = GL_CONSTANT_EXT,
#endif
#ifdef GL_CONSTANT_NV
    constant_nv = GL_CONSTANT_NV,
#endif
#ifdef GL_INTERPOLATE
    interpolate = GL_INTERPOLATE,
#endif
#ifdef GL_INTERPOLATE_ARB
    interpolate_arb = GL_INTERPOLATE_ARB,
#endif
#ifdef GL_INTERPOLATE_EXT
    interpolate_ext = GL_INTERPOLATE_EXT,
#endif
#ifdef GL_OPERAND0_ALPHA
    operand0_alpha = GL_OPERAND0_ALPHA,
#endif
#ifdef GL_OPERAND0_ALPHA_ARB
    operand0_alpha_arb = GL_OPERAND0_ALPHA_ARB,
#endif
#ifdef GL_OPERAND0_ALPHA_EXT
    operand0_alpha_ext = GL_OPERAND0_ALPHA_EXT,
#endif
#ifdef GL_OPERAND0_RGB
    operand0_rgb = GL_OPERAND0_RGB,
#endif
#ifdef GL_OPERAND0_RGB_ARB
    operand0_rgb_arb = GL_OPERAND0_RGB_ARB,
#endif
#ifdef GL_OPERAND0_RGB_EXT
    operand0_rgb_ext = GL_OPERAND0_RGB_EXT,
#endif
#ifdef GL_OPERAND1_ALPHA
    operand1_alpha = GL_OPERAND1_ALPHA,
#endif
#ifdef GL_OPERAND1_ALPHA_ARB
    operand1_alpha_arb = GL_OPERAND1_ALPHA_ARB,
#endif
#ifdef GL_OPERAND1_ALPHA_EXT
    operand1_alpha_ext = GL_OPERAND1_ALPHA_EXT,
#endif
#ifdef GL_OPERAND1_RGB
    operand1_rgb = GL_OPERAND1_RGB,
#endif
#ifdef GL_OPERAND1_RGB_ARB
    operand1_rgb_arb = GL_OPERAND1_RGB_ARB,
#endif
#ifdef GL_OPERAND1_RGB_EXT
    operand1_rgb_ext = GL_OPERAND1_RGB_EXT,
#endif
#ifdef GL_OPERAND2_ALPHA
    operand2_alpha = GL_OPERAND2_ALPHA,
#endif
#ifdef GL_OPERAND2_ALPHA_ARB
    operand2_alpha_arb = GL_OPERAND2_ALPHA_ARB,
#endif
#ifdef GL_OPERAND2_ALPHA_EXT
    operand2_alpha_ext = GL_OPERAND2_ALPHA_EXT,
#endif
#ifdef GL_OPERAND2_RGB
    operand2_rgb = GL_OPERAND2_RGB,
#endif
#ifdef GL_OPERAND2_RGB_ARB
    operand2_rgb_arb = GL_OPERAND2_RGB_ARB,
#endif
#ifdef GL_OPERAND2_RGB_EXT
    operand2_rgb_ext = GL_OPERAND2_RGB_EXT,
#endif
#ifdef GL_OPERAND3_ALPHA_NV
    operand3_alpha_nv = GL_OPERAND3_ALPHA_NV,
#endif
#ifdef GL_OPERAND3_RGB_NV
    operand3_rgb_nv = GL_OPERAND3_RGB_NV,
#endif
#ifdef GL_PREVIOUS
    previous = GL_PREVIOUS,
#endif
#ifdef GL_PREVIOUS_ARB
    previous_arb = GL_PREVIOUS_ARB,
#endif
#ifdef GL_PREVIOUS_EXT
    previous_ext = GL_PREVIOUS_EXT,
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
#ifdef GL_RGB_SCALE
    rgb_scale = GL_RGB_SCALE,
#endif
#ifdef GL_RGB_SCALE_ARB
    rgb_scale_arb = GL_RGB_SCALE_ARB,
#endif
#ifdef GL_RGB_SCALE_EXT
    rgb_scale_ext = GL_RGB_SCALE_EXT,
#endif
#ifdef GL_SOURCE0_ALPHA
    source0_alpha = GL_SOURCE0_ALPHA,
#endif
#ifdef GL_SOURCE0_ALPHA_ARB
    source0_alpha_arb = GL_SOURCE0_ALPHA_ARB,
#endif
#ifdef GL_SOURCE0_ALPHA_EXT
    source0_alpha_ext = GL_SOURCE0_ALPHA_EXT,
#endif
#ifdef GL_SOURCE0_RGB
    source0_rgb = GL_SOURCE0_RGB,
#endif
#ifdef GL_SOURCE0_RGB_ARB
    source0_rgb_arb = GL_SOURCE0_RGB_ARB,
#endif
#ifdef GL_SOURCE0_RGB_EXT
    source0_rgb_ext = GL_SOURCE0_RGB_EXT,
#endif
#ifdef GL_SOURCE1_ALPHA
    source1_alpha = GL_SOURCE1_ALPHA,
#endif
#ifdef GL_SOURCE1_ALPHA_ARB
    source1_alpha_arb = GL_SOURCE1_ALPHA_ARB,
#endif
#ifdef GL_SOURCE1_ALPHA_EXT
    source1_alpha_ext = GL_SOURCE1_ALPHA_EXT,
#endif
#ifdef GL_SOURCE1_RGB
    source1_rgb = GL_SOURCE1_RGB,
#endif
#ifdef GL_SOURCE1_RGB_ARB
    source1_rgb_arb = GL_SOURCE1_RGB_ARB,
#endif
#ifdef GL_SOURCE1_RGB_EXT
    source1_rgb_ext = GL_SOURCE1_RGB_EXT,
#endif
#ifdef GL_SOURCE2_ALPHA
    source2_alpha = GL_SOURCE2_ALPHA,
#endif
#ifdef GL_SOURCE2_ALPHA_ARB
    source2_alpha_arb = GL_SOURCE2_ALPHA_ARB,
#endif
#ifdef GL_SOURCE2_ALPHA_EXT
    source2_alpha_ext = GL_SOURCE2_ALPHA_EXT,
#endif
#ifdef GL_SOURCE2_RGB
    source2_rgb = GL_SOURCE2_RGB,
#endif
#ifdef GL_SOURCE2_RGB_ARB
    source2_rgb_arb = GL_SOURCE2_RGB_ARB,
#endif
#ifdef GL_SOURCE2_RGB_EXT
    source2_rgb_ext = GL_SOURCE2_RGB_EXT,
#endif
#ifdef GL_SOURCE3_ALPHA_NV
    source3_alpha_nv = GL_SOURCE3_ALPHA_NV,
#endif
#ifdef GL_SOURCE3_RGB_NV
    source3_rgb_nv = GL_SOURCE3_RGB_NV,
#endif
#ifdef GL_SRC0_ALPHA
    src0_alpha = GL_SRC0_ALPHA,
#endif
#ifdef GL_SRC0_RGB
    src0_rgb = GL_SRC0_RGB,
#endif
#ifdef GL_SRC1_ALPHA
    src1_alpha = GL_SRC1_ALPHA,
#endif
#ifdef GL_SRC1_ALPHA_EXT
    src1_alpha_ext = GL_SRC1_ALPHA_EXT,
#endif
#ifdef GL_SRC1_RGB
    src1_rgb = GL_SRC1_RGB,
#endif
#ifdef GL_SRC2_ALPHA
    src2_alpha = GL_SRC2_ALPHA,
#endif
#ifdef GL_SRC2_RGB
    src2_rgb = GL_SRC2_RGB,
#endif
#ifdef GL_TEXTURE_ENV_COLOR
    texture_env_color = GL_TEXTURE_ENV_COLOR,
#endif
#ifdef GL_TEXTURE_ENV_MODE
    texture_env_mode = GL_TEXTURE_ENV_MODE,
#endif
}; // enum class texture_env_parameter

} // namespace gl::group
