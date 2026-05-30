#pragma once

#ifdef GL_NV_pack_subimage
#include "../enums/GetPName.h"
#include "../enums/PixelStoreParameter.h"

namespace gl::nv::pack_subimage {
using gl::group::get_prop;
using gl::group::pixel_store_parameter;

namespace values {
} // namespace values
} // namespace gl::nv::pack_subimage
#endif // GL_NV_pack_subimage
namespace gl::nv::pack_subimage {
constexpr auto name = "GL_NV_pack_subimage";
}
