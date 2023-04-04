#pragma once

#include "common.h"

namespace gl::group {

// TransformFeedbackPName
enum class transform_feedback_prop : u32
{
#ifdef GL_TRANSFORM_FEEDBACK_ACTIVE
    transform_feedback_active = GL_TRANSFORM_FEEDBACK_ACTIVE,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER_BINDING
    transform_feedback_buffer_binding = GL_TRANSFORM_FEEDBACK_BUFFER_BINDING,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER_SIZE
    transform_feedback_buffer_size = GL_TRANSFORM_FEEDBACK_BUFFER_SIZE,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER_START
    transform_feedback_buffer_start = GL_TRANSFORM_FEEDBACK_BUFFER_START,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_PAUSED
    transform_feedback_paused = GL_TRANSFORM_FEEDBACK_PAUSED,
#endif
}; // enum class transform_feedback_prop

} // namespace gl::group
