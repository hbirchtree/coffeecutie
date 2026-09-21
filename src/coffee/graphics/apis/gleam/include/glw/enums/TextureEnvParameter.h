#pragma once

#include "common.h"

namespace gl::group {

// TextureEnvParameter
enum class texture_env_parameter : u32
{
    add_signed         = 0x8574, // GL_ADD_SIGNED
    alpha_scale        = 0x0D1C, // GL_ALPHA_SCALE
    combine            = 0x8570, // GL_COMBINE
    combine_alpha      = 0x8572, // GL_COMBINE_ALPHA
    combine_rgb        = 0x8571, // GL_COMBINE_RGB
    constant           = 0x8576, // GL_CONSTANT
    coord_replace      = 0x8862, // GL_COORD_REPLACE
    interpolate        = 0x8575, // GL_INTERPOLATE
    operand0_alpha     = 0x8598, // GL_OPERAND0_ALPHA
    operand0_rgb       = 0x8590, // GL_OPERAND0_RGB
    operand1_alpha     = 0x8599, // GL_OPERAND1_ALPHA
    operand1_rgb       = 0x8591, // GL_OPERAND1_RGB
    operand2_alpha     = 0x859A, // GL_OPERAND2_ALPHA
    operand2_rgb       = 0x8592, // GL_OPERAND2_RGB
    previous           = 0x8578, // GL_PREVIOUS
    primary_color      = 0x8577, // GL_PRIMARY_COLOR
    rgb_scale          = 0x8573, // GL_RGB_SCALE
    source0_alpha      = 0x8588, // GL_SOURCE0_ALPHA
    source0_rgb        = 0x8580, // GL_SOURCE0_RGB
    source1_alpha      = 0x8589, // GL_SOURCE1_ALPHA
    source1_rgb        = 0x8581, // GL_SOURCE1_RGB
    source2_alpha      = 0x858A, // GL_SOURCE2_ALPHA
    source2_rgb        = 0x8582, // GL_SOURCE2_RGB
    src0_alpha         = 0x8588, // GL_SRC0_ALPHA
    src0_rgb           = 0x8580, // GL_SRC0_RGB
    src1_alpha         = 0x8589, // GL_SRC1_ALPHA
    src1_rgb           = 0x8581, // GL_SRC1_RGB
    src2_alpha         = 0x858A, // GL_SRC2_ALPHA
    src2_rgb           = 0x8582, // GL_SRC2_RGB
    texture_env_color  = 0x2201, // GL_TEXTURE_ENV_COLOR
    texture_env_mode   = 0x2200, // GL_TEXTURE_ENV_MODE
    texture_lod_bias   = 0x8501, // GL_TEXTURE_LOD_BIAS
    add_signed_arb     = 0x8574, // GL_ADD_SIGNED_ARB
    add_signed_ext     = 0x8574, // GL_ADD_SIGNED_EXT
    combine_alpha_arb  = 0x8572, // GL_COMBINE_ALPHA_ARB
    combine_alpha_ext  = 0x8572, // GL_COMBINE_ALPHA_EXT
    combine_arb        = 0x8570, // GL_COMBINE_ARB
    combine_ext        = 0x8570, // GL_COMBINE_EXT
    combine_rgb_arb    = 0x8571, // GL_COMBINE_RGB_ARB
    combine_rgb_ext    = 0x8571, // GL_COMBINE_RGB_EXT
    constant_arb       = 0x8576, // GL_CONSTANT_ARB
    constant_ext       = 0x8576, // GL_CONSTANT_EXT
    constant_nv        = 0x8576, // GL_CONSTANT_NV
    interpolate_arb    = 0x8575, // GL_INTERPOLATE_ARB
    interpolate_ext    = 0x8575, // GL_INTERPOLATE_EXT
    operand0_alpha_arb = 0x8598, // GL_OPERAND0_ALPHA_ARB
    operand0_alpha_ext = 0x8598, // GL_OPERAND0_ALPHA_EXT
    operand0_rgb_arb   = 0x8590, // GL_OPERAND0_RGB_ARB
    operand0_rgb_ext   = 0x8590, // GL_OPERAND0_RGB_EXT
    operand1_alpha_arb = 0x8599, // GL_OPERAND1_ALPHA_ARB
    operand1_alpha_ext = 0x8599, // GL_OPERAND1_ALPHA_EXT
    operand1_rgb_arb   = 0x8591, // GL_OPERAND1_RGB_ARB
    operand1_rgb_ext   = 0x8591, // GL_OPERAND1_RGB_EXT
    operand2_alpha_arb = 0x859A, // GL_OPERAND2_ALPHA_ARB
    operand2_alpha_ext = 0x859A, // GL_OPERAND2_ALPHA_EXT
    operand2_rgb_arb   = 0x8592, // GL_OPERAND2_RGB_ARB
    operand2_rgb_ext   = 0x8592, // GL_OPERAND2_RGB_EXT
    operand3_alpha_nv  = 0x859B, // GL_OPERAND3_ALPHA_NV
    operand3_rgb_nv    = 0x8593, // GL_OPERAND3_RGB_NV
    previous_arb       = 0x8578, // GL_PREVIOUS_ARB
    previous_ext       = 0x8578, // GL_PREVIOUS_EXT
    primary_color_arb  = 0x8577, // GL_PRIMARY_COLOR_ARB
    primary_color_ext  = 0x8577, // GL_PRIMARY_COLOR_EXT
    rgb_scale_arb      = 0x8573, // GL_RGB_SCALE_ARB
    rgb_scale_ext      = 0x8573, // GL_RGB_SCALE_EXT
    source0_alpha_arb  = 0x8588, // GL_SOURCE0_ALPHA_ARB
    source0_alpha_ext  = 0x8588, // GL_SOURCE0_ALPHA_EXT
    source0_rgb_arb    = 0x8580, // GL_SOURCE0_RGB_ARB
    source0_rgb_ext    = 0x8580, // GL_SOURCE0_RGB_EXT
    source1_alpha_arb  = 0x8589, // GL_SOURCE1_ALPHA_ARB
    source1_alpha_ext  = 0x8589, // GL_SOURCE1_ALPHA_EXT
    source1_rgb_arb    = 0x8581, // GL_SOURCE1_RGB_ARB
    source1_rgb_ext    = 0x8581, // GL_SOURCE1_RGB_EXT
    source2_alpha_arb  = 0x858A, // GL_SOURCE2_ALPHA_ARB
    source2_alpha_ext  = 0x858A, // GL_SOURCE2_ALPHA_EXT
    source2_rgb_arb    = 0x8582, // GL_SOURCE2_RGB_ARB
    source2_rgb_ext    = 0x8582, // GL_SOURCE2_RGB_EXT
    source3_alpha_nv   = 0x858B, // GL_SOURCE3_ALPHA_NV
    source3_rgb_nv     = 0x8583, // GL_SOURCE3_RGB_NV
    src1_alpha_ext     = 0x8589, // GL_SRC1_ALPHA_EXT
}; // enum class texture_env_parameter

} // namespace gl::group
