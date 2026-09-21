#pragma once

#include "common.h"

namespace gl::group {

// TransformFeedbackTokenNV
enum class transform_feedback_token_nv : i32
{
    next_buffer_nv      = -2, // GL_NEXT_BUFFER_NV
    skip_components1_nv = -6, // GL_SKIP_COMPONENTS1_NV
    skip_components2_nv = -5, // GL_SKIP_COMPONENTS2_NV
    skip_components3_nv = -4, // GL_SKIP_COMPONENTS3_NV
    skip_components4_nv = -3, // GL_SKIP_COMPONENTS4_NV
}; // enum class transform_feedback_token_nv

} // namespace gl::group
