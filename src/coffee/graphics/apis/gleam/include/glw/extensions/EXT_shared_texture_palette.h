#pragma once

#ifdef GL_EXT_shared_texture_palette
#include "../enums/EnableCap.h"
#include "../enums/GetPName.h"
namespace gl::ext::shared_texture_palette {
using gl::group::enable_cap;
using gl::group::get_prop;
namespace values {
} // namespace values
} // namespace gl::ext::shared_texture_palette
#endif // GL_EXT_shared_texture_palette
namespace gl::ext::shared_texture_palette {
constexpr auto name = "GL_EXT_shared_texture_palette";
}
