#pragma once

#ifdef GL_OES_depth24
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"

namespace gl::oes::depth24 {
using gl::group::internal_format;
using gl::group::sized_internal_format;

namespace values {
} // namespace values
} // namespace gl::oes::depth24
#endif // GL_OES_depth24
namespace gl::oes::depth24 {
constexpr auto name = "GL_OES_depth24";
}
