#pragma once

#ifdef GL_EXT_render_snorm
#include "../enums/BinormalPointerTypeEXT.h"
#include "../enums/ColorPointerType.h"
#include "../enums/IndexPointerType.h"
#include "../enums/InternalFormat.h"
#include "../enums/ListNameType.h"
#include "../enums/NormalPointerType.h"
#include "../enums/PixelType.h"
#include "../enums/SecondaryColorPointerTypeIBM.h"
#include "../enums/SizedInternalFormat.h"
#include "../enums/TangentPointerTypeEXT.h"
#include "../enums/TexCoordPointerType.h"
#include "../enums/VertexAttribIType.h"
#include "../enums/VertexAttribPointerType.h"
#include "../enums/VertexAttribType.h"
#include "../enums/VertexPointerType.h"
#include "../enums/WeightPointerTypeARB.h"

namespace gl::ext::render_snorm {
using gl::group::binormal_pointer_type_ext;
using gl::group::color_pointer_type;
using gl::group::index_pointer_type;
using gl::group::internal_format;
using gl::group::list_name_type;
using gl::group::normal_pointer_type;
using gl::group::pixel_type;
using gl::group::secondary_color_pointer_type_ibm;
using gl::group::sized_internal_format;
using gl::group::tangent_pointer_type_ext;
using gl::group::tex_coord_pointer_type;
using gl::group::vertex_attrib_int;
using gl::group::vertex_attrib_pointer_type;
using gl::group::vertex_attrib_type;
using gl::group::vertex_pointer_type;
using gl::group::weight_pointer_type_arb;

namespace values {
} // namespace values
} // namespace gl::ext::render_snorm
#endif // GL_EXT_render_snorm
namespace gl::ext::render_snorm {
constexpr auto name = "GL_EXT_render_snorm";
}
