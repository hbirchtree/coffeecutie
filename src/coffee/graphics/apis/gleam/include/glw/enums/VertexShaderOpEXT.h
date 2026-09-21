#pragma once

#include "common.h"

namespace gl::group {

// VertexShaderOpEXT
enum class vertex_shader_op_ext : u32
{
    op_add_ext             = 0x8787, // GL_OP_ADD_EXT
    op_clamp_ext           = 0x878E, // GL_OP_CLAMP_EXT
    op_cross_product_ext   = 0x8797, // GL_OP_CROSS_PRODUCT_EXT
    op_dot3_ext            = 0x8784, // GL_OP_DOT3_EXT
    op_dot4_ext            = 0x8785, // GL_OP_DOT4_EXT
    op_exp_base_2_ext      = 0x8791, // GL_OP_EXP_BASE_2_EXT
    op_floor_ext           = 0x878F, // GL_OP_FLOOR_EXT
    op_frac_ext            = 0x8789, // GL_OP_FRAC_EXT
    op_index_ext           = 0x8782, // GL_OP_INDEX_EXT
    op_log_base_2_ext      = 0x8792, // GL_OP_LOG_BASE_2_EXT
    op_madd_ext            = 0x8788, // GL_OP_MADD_EXT
    op_max_ext             = 0x878A, // GL_OP_MAX_EXT
    op_min_ext             = 0x878B, // GL_OP_MIN_EXT
    op_mov_ext             = 0x8799, // GL_OP_MOV_EXT
    op_multiply_matrix_ext = 0x8798, // GL_OP_MULTIPLY_MATRIX_EXT
    op_mul_ext             = 0x8786, // GL_OP_MUL_EXT
    op_negate_ext          = 0x8783, // GL_OP_NEGATE_EXT
    op_power_ext           = 0x8793, // GL_OP_POWER_EXT
    op_recip_ext           = 0x8794, // GL_OP_RECIP_EXT
    op_recip_sqrt_ext      = 0x8795, // GL_OP_RECIP_SQRT_EXT
    op_round_ext           = 0x8790, // GL_OP_ROUND_EXT
    op_set_ge_ext          = 0x878C, // GL_OP_SET_GE_EXT
    op_set_lt_ext          = 0x878D, // GL_OP_SET_LT_EXT
    op_sub_ext             = 0x8796, // GL_OP_SUB_EXT
}; // enum class vertex_shader_op_ext

} // namespace gl::group
