#pragma once

#include "common.h"

namespace gl::group {

// TransformFeedbackPName
enum class transform_feedback_prop : u32
{
    transform_feedback_active = 0x8E24, // GL_TRANSFORM_FEEDBACK_ACTIVE
    transform_feedback_buffer_binding =
        0x8C8F, // GL_TRANSFORM_FEEDBACK_BUFFER_BINDING
    transform_feedback_buffer_size =
        0x8C85, // GL_TRANSFORM_FEEDBACK_BUFFER_SIZE
    transform_feedback_buffer_start =
        0x8C84,                         // GL_TRANSFORM_FEEDBACK_BUFFER_START
    transform_feedback_paused = 0x8E23, // GL_TRANSFORM_FEEDBACK_PAUSED
}; // enum class transform_feedback_prop

} // namespace gl::group
