#pragma once

#include "common.h"

namespace gl::group {

// VertexShaderOpEXT
enum class vertex_shader_op_ext : u32
{
#ifdef GL_OP_ADD_EXT
    op_add_ext = GL_OP_ADD_EXT,
#endif
#ifdef GL_OP_CLAMP_EXT
    op_clamp_ext = GL_OP_CLAMP_EXT,
#endif
#ifdef GL_OP_CROSS_PRODUCT_EXT
    op_cross_product_ext = GL_OP_CROSS_PRODUCT_EXT,
#endif
#ifdef GL_OP_DOT3_EXT
    op_dot3_ext = GL_OP_DOT3_EXT,
#endif
#ifdef GL_OP_DOT4_EXT
    op_dot4_ext = GL_OP_DOT4_EXT,
#endif
#ifdef GL_OP_EXP_BASE_2_EXT
    op_exp_base_2_ext = GL_OP_EXP_BASE_2_EXT,
#endif
#ifdef GL_OP_FLOOR_EXT
    op_floor_ext = GL_OP_FLOOR_EXT,
#endif
#ifdef GL_OP_FRAC_EXT
    op_frac_ext = GL_OP_FRAC_EXT,
#endif
#ifdef GL_OP_INDEX_EXT
    op_index_ext = GL_OP_INDEX_EXT,
#endif
#ifdef GL_OP_LOG_BASE_2_EXT
    op_log_base_2_ext = GL_OP_LOG_BASE_2_EXT,
#endif
#ifdef GL_OP_MADD_EXT
    op_madd_ext = GL_OP_MADD_EXT,
#endif
#ifdef GL_OP_MAX_EXT
    op_max_ext = GL_OP_MAX_EXT,
#endif
#ifdef GL_OP_MIN_EXT
    op_min_ext = GL_OP_MIN_EXT,
#endif
#ifdef GL_OP_MOV_EXT
    op_mov_ext = GL_OP_MOV_EXT,
#endif
#ifdef GL_OP_MULTIPLY_MATRIX_EXT
    op_multiply_matrix_ext = GL_OP_MULTIPLY_MATRIX_EXT,
#endif
#ifdef GL_OP_MUL_EXT
    op_mul_ext = GL_OP_MUL_EXT,
#endif
#ifdef GL_OP_NEGATE_EXT
    op_negate_ext = GL_OP_NEGATE_EXT,
#endif
#ifdef GL_OP_POWER_EXT
    op_power_ext = GL_OP_POWER_EXT,
#endif
#ifdef GL_OP_RECIP_EXT
    op_recip_ext = GL_OP_RECIP_EXT,
#endif
#ifdef GL_OP_RECIP_SQRT_EXT
    op_recip_sqrt_ext = GL_OP_RECIP_SQRT_EXT,
#endif
#ifdef GL_OP_ROUND_EXT
    op_round_ext = GL_OP_ROUND_EXT,
#endif
#ifdef GL_OP_SET_GE_EXT
    op_set_ge_ext = GL_OP_SET_GE_EXT,
#endif
#ifdef GL_OP_SET_LT_EXT
    op_set_lt_ext = GL_OP_SET_LT_EXT,
#endif
#ifdef GL_OP_SUB_EXT
    op_sub_ext = GL_OP_SUB_EXT,
#endif
}; // enum class vertex_shader_op_ext

} // namespace gl::group
