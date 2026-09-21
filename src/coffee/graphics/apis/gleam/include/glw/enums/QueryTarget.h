#pragma once

#include "common.h"

namespace gl::group {

// QueryTarget
enum class query_target : u32
{
    any_samples_passed = 0x8C2F, // GL_ANY_SAMPLES_PASSED
    any_samples_passed_conservative =
        0x8D6A,                           // GL_ANY_SAMPLES_PASSED_CONSERVATIVE
    primitives_generated        = 0x8C87, // GL_PRIMITIVES_GENERATED
    primitives_submitted        = 0x82EF, // GL_PRIMITIVES_SUBMITTED
    samples_passed              = 0x8914, // GL_SAMPLES_PASSED
    time_elapsed                = 0x88BF, // GL_TIME_ELAPSED
    transform_feedback_overflow = 0x82EC, // GL_TRANSFORM_FEEDBACK_OVERFLOW
    transform_feedback_primitives_written =
        0x8C88, // GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN
    vertex_shader_invocations     = 0x82F0, // GL_VERTEX_SHADER_INVOCATIONS
    vertices_submitted            = 0x82EE, // GL_VERTICES_SUBMITTED
    mesh_primitives_generated_ext = 0x9755, // GL_MESH_PRIMITIVES_GENERATED_EXT
    mesh_shader_invocations_ext   = 0x9754, // GL_MESH_SHADER_INVOCATIONS_EXT
    task_shader_invocations_ext   = 0x9753, // GL_TASK_SHADER_INVOCATIONS_EXT
}; // enum class query_target

} // namespace gl::group
