#pragma once

#ifdef GL_EXT_rescale_normal
#include "../enums/EnableCap.h"
#include "../enums/GetPName.h"

namespace gl::ext::rescale_normal {
using gl::group::enable_cap;
using gl::group::get_prop;

namespace values {
} // namespace values
} // namespace gl::ext::rescale_normal
#endif // GL_EXT_rescale_normal
namespace gl::ext::rescale_normal {
constexpr auto name = "GL_EXT_rescale_normal";
}
