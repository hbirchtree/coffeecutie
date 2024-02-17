#pragma once

#ifdef GL_EXT_unpack_subimage
#include "../enums/PixelStoreParameter.h"

namespace gl::ext::unpack_subimage {
using gl::group::pixel_store_parameter;

namespace values {
} // namespace values
} // namespace gl::ext::unpack_subimage
#endif // GL_EXT_unpack_subimage
namespace gl::ext::unpack_subimage {
constexpr auto name = "GL_EXT_unpack_subimage";
}
