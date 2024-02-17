#pragma once

#ifdef GL_OES_depth32
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"

namespace gl::oes::depth32 {
using gl::group::internal_format;
using gl::group::sized_internal_format;

namespace values {
} // namespace values
} // namespace gl::oes::depth32
#endif // GL_OES_depth32
namespace gl::oes::depth32 {
constexpr auto name = "GL_OES_depth32";
}
