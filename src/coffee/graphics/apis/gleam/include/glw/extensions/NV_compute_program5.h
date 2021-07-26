#pragma once

#ifdef GL_NV_compute_program5
#include "../enums/ProgramTarget.h"
namespace gl::nv::compute_program5 {
using gl::group::program_target;
namespace values {
constexpr libc_types::u32 compute_program_parameter_buffer = 0x90FC;
} // namespace values
} // namespace gl::nv::compute_program5
#endif // GL_NV_compute_program5
namespace gl::nv::compute_program5 {
constexpr auto name = "GL_NV_compute_program5";
}
