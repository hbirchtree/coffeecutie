#pragma once

#include "common.h"

namespace gl::group {

// BindTransformFeedbackTarget
enum class bind_transform_feedback_target : u32
{
#ifdef GL_TRANSFORM_FEEDBACK
    transform_feedback = GL_TRANSFORM_FEEDBACK,
#endif
}; // enum class bind_transform_feedback_target

} // namespace gl::group
