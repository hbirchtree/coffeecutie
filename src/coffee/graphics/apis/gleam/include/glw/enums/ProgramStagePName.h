#pragma once

#include "common.h"

namespace gl::group {

// ProgramStagePName
enum class program_stage_prop : u32
{
#ifdef GL_ACTIVE_SUBROUTINES
    active_subroutines = GL_ACTIVE_SUBROUTINES,
#endif
#ifdef GL_ACTIVE_SUBROUTINE_MAX_LENGTH
    active_subroutine_max_length = GL_ACTIVE_SUBROUTINE_MAX_LENGTH,
#endif
#ifdef GL_ACTIVE_SUBROUTINE_UNIFORMS
    active_subroutine_uniforms = GL_ACTIVE_SUBROUTINE_UNIFORMS,
#endif
#ifdef GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS
    active_subroutine_uniform_locations =
        GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
#endif
#ifdef GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH
    active_subroutine_uniform_max_length =
        GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH,
#endif
}; // enum class program_stage_prop

} // namespace gl::group
