#pragma once

#ifdef GL_EXT_cmyka
#include "../enums/GetPName.h"
#include "../enums/HintTarget.h"
#include "../enums/PixelFormat.h"

namespace gl::ext::cmyka {
using gl::group::get_prop;
using gl::group::hint_target;
using gl::group::pixel_format;

namespace values {
} // namespace values
} // namespace gl::ext::cmyka
#endif // GL_EXT_cmyka
namespace gl::ext::cmyka {
constexpr auto name = "GL_EXT_cmyka";
}
