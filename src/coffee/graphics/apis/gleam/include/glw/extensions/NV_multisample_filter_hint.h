#pragma once

#ifdef GL_NV_multisample_filter_hint
#include "../enums/HintTarget.h"

namespace gl::nv::multisample_filter_hint {
using gl::group::hint_target;

namespace values {
} // namespace values
} // namespace gl::nv::multisample_filter_hint
#endif // GL_NV_multisample_filter_hint
namespace gl::nv::multisample_filter_hint {
constexpr auto name = "GL_NV_multisample_filter_hint";
}
