#pragma once

#ifdef GL_NV_tessellation_program5
#include "../enums/ProgramTarget.h"

namespace gl::nv::tessellation_program5 {
using gl::group::program_target;

namespace values {
constexpr u32 max_program_patch_attribs                = 0x86D8;
constexpr u32 tess_control_program_parameter_buffer    = 0x8C74;
constexpr u32 tess_evaluation_program_parameter_buffer = 0x8C75;
} // namespace values
} // namespace gl::nv::tessellation_program5
#endif // GL_NV_tessellation_program5
namespace gl::nv::tessellation_program5 {
constexpr auto name = "GL_NV_tessellation_program5";
}
