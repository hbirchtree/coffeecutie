#pragma once

#include "common.h"

namespace gl::group {

// ShadingRateCombinerOp
enum class shading_rate_combiner_op : u32
{
#ifdef GL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_EXT
    fragment_shading_rate_combiner_op_keep_ext =
        GL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_EXT,
#endif
#ifdef GL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_EXT
    fragment_shading_rate_combiner_op_max_ext =
        GL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_EXT,
#endif
#ifdef GL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_EXT
    fragment_shading_rate_combiner_op_min_ext =
        GL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_EXT,
#endif
#ifdef GL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_EXT
    fragment_shading_rate_combiner_op_mul_ext =
        GL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_EXT,
#endif
#ifdef GL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_EXT
    fragment_shading_rate_combiner_op_replace_ext =
        GL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_EXT,
#endif
}; // enum class shading_rate_combiner_op

} // namespace gl::group
