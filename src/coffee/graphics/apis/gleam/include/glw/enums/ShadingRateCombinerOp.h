#pragma once

#include "common.h"

namespace gl::group {

// ShadingRateCombinerOp
enum class shading_rate_combiner_op : u32
{
    fragment_shading_rate_combiner_op_keep_ext =
        0x96D2, // GL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_EXT
    fragment_shading_rate_combiner_op_max_ext =
        0x96D5, // GL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_EXT
    fragment_shading_rate_combiner_op_min_ext =
        0x96D4, // GL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_EXT
    fragment_shading_rate_combiner_op_mul_ext =
        0x96D6, // GL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_EXT
    fragment_shading_rate_combiner_op_replace_ext =
        0x96D3, // GL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_EXT
}; // enum class shading_rate_combiner_op

} // namespace gl::group
