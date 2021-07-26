#pragma once

#ifdef GL_OES_required_internalformat
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::oes::required_internalformat {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
constexpr libc_types::u32 rgb565 = 0x8D62;
} // namespace values
} // namespace gl::oes::required_internalformat
#endif // GL_OES_required_internalformat
namespace gl::oes::required_internalformat {
constexpr auto name = "GL_OES_required_internalformat";
}
