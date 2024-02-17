#pragma once

#ifdef GL_EXT_EGL_image_storage_compression
#include "../enums/GetTextureParameter.h"
#include "../enums/TexStorageAttribs.h"

namespace gl::ext::egl_image_storage_compression {
using gl::group::get_texture_parameter;
using gl::group::tex_storage_attribs;

namespace values {
} // namespace values
} // namespace gl::ext::egl_image_storage_compression
#endif // GL_EXT_EGL_image_storage_compression
namespace gl::ext::egl_image_storage_compression {
constexpr auto name = "GL_EXT_EGL_image_storage_compression";
}
