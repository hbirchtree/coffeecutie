#pragma once

#include "common.h"

namespace gl::group {

// TransformFeedbackBufferMode
enum class transform_feedback_buffer_mode : u32
{
#ifdef GL_INTERLEAVED_ATTRIBS
    interleaved_attribs = GL_INTERLEAVED_ATTRIBS,
#endif
#ifdef GL_SEPARATE_ATTRIBS
    separate_attribs = GL_SEPARATE_ATTRIBS,
#endif
}; // enum class transform_feedback_buffer_mode

} // namespace gl::group
