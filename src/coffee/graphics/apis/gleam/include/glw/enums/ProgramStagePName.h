#pragma once

#include "common.h"

namespace gl::group {

// ProgramStagePName
enum class program_stage_prop : u32
{
    active_subroutines           = 0x8DE5, // GL_ACTIVE_SUBROUTINES
    active_subroutine_max_length = 0x8E48, // GL_ACTIVE_SUBROUTINE_MAX_LENGTH
    active_subroutine_uniforms   = 0x8DE6, // GL_ACTIVE_SUBROUTINE_UNIFORMS
    active_subroutine_uniform_locations =
        0x8E47, // GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS
    active_subroutine_uniform_max_length =
        0x8E49, // GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH
}; // enum class program_stage_prop

} // namespace gl::group
