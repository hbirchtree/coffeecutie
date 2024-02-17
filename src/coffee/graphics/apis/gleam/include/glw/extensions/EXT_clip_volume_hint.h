#pragma once

#ifdef GL_EXT_clip_volume_hint
#include "../enums/HintTarget.h"

namespace gl::ext::clip_volume_hint {
using gl::group::hint_target;

namespace values {
} // namespace values
} // namespace gl::ext::clip_volume_hint
#endif // GL_EXT_clip_volume_hint
namespace gl::ext::clip_volume_hint {
constexpr auto name = "GL_EXT_clip_volume_hint";
}
