#pragma once

#include "common.h"

namespace gl::group {

// TransformFeedbackTokenNV
enum class transform_feedback_token_nv : ::libc_types::u32
{
#ifdef GL_NEXT_BUFFER_NV
    next_buffer_nv = GL_NEXT_BUFFER_NV,
#endif
#ifdef GL_SKIP_COMPONENTS1_NV
    skip_components1_nv = GL_SKIP_COMPONENTS1_NV,
#endif
#ifdef GL_SKIP_COMPONENTS2_NV
    skip_components2_nv = GL_SKIP_COMPONENTS2_NV,
#endif
#ifdef GL_SKIP_COMPONENTS3_NV
    skip_components3_nv = GL_SKIP_COMPONENTS3_NV,
#endif
#ifdef GL_SKIP_COMPONENTS4_NV
    skip_components4_nv = GL_SKIP_COMPONENTS4_NV,
#endif
}; // enum class transform_feedback_token_nv

} // namespace gl::group
