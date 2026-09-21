#pragma once

#include "common.h"

namespace gl::group {

// TransformFeedbackBufferMode
enum class transform_feedback_buffer_mode : u32
{
    interleaved_attribs = 0x8C8C, // GL_INTERLEAVED_ATTRIBS
    separate_attribs    = 0x8C8D, // GL_SEPARATE_ATTRIBS
}; // enum class transform_feedback_buffer_mode

} // namespace gl::group
