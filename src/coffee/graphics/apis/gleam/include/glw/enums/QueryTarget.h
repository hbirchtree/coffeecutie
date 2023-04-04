#pragma once

#include "common.h"

namespace gl::group {

// QueryTarget
enum class query_target : u32
{
#ifdef GL_ANY_SAMPLES_PASSED
    any_samples_passed = GL_ANY_SAMPLES_PASSED,
#endif
#ifdef GL_ANY_SAMPLES_PASSED_CONSERVATIVE
    any_samples_passed_conservative = GL_ANY_SAMPLES_PASSED_CONSERVATIVE,
#endif
#ifdef GL_PRIMITIVES_GENERATED
    primitives_generated = GL_PRIMITIVES_GENERATED,
#endif
#ifdef GL_PRIMITIVES_SUBMITTED
    primitives_submitted = GL_PRIMITIVES_SUBMITTED,
#endif
#ifdef GL_SAMPLES_PASSED
    samples_passed = GL_SAMPLES_PASSED,
#endif
#ifdef GL_TIME_ELAPSED
    time_elapsed = GL_TIME_ELAPSED,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_OVERFLOW
    transform_feedback_overflow = GL_TRANSFORM_FEEDBACK_OVERFLOW,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN
    transform_feedback_primitives_written =
        GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
#endif
#ifdef GL_VERTEX_SHADER_INVOCATIONS
    vertex_shader_invocations = GL_VERTEX_SHADER_INVOCATIONS,
#endif
#ifdef GL_VERTICES_SUBMITTED
    vertices_submitted = GL_VERTICES_SUBMITTED,
#endif
}; // enum class query_target

} // namespace gl::group
