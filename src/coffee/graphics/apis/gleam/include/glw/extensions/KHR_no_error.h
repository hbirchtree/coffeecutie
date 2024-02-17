#pragma once

#ifdef GL_KHR_no_error
#include "../enums/ContextFlagMask.h"

namespace gl::khr::no_error {
using gl::group::context_flag_mask;

namespace values {
} // namespace values
} // namespace gl::khr::no_error
#endif // GL_KHR_no_error
namespace gl::khr::no_error {
constexpr auto name = "GL_KHR_no_error";
}
