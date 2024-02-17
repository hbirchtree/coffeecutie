#pragma once

#ifdef GL_OES_depth_texture
#include "../enums/AttributeType.h"
#include "../enums/ColorPointerType.h"
#include "../enums/DepthStencilTextureMode.h"
#include "../enums/DrawElementsType.h"
#include "../enums/ElementPointerTypeATI.h"
#include "../enums/InternalFormat.h"
#include "../enums/ListNameType.h"
#include "../enums/MatrixIndexPointerTypeARB.h"
#include "../enums/PixelFormat.h"
#include "../enums/PixelType.h"
#include "../enums/ReplacementCodeTypeSUN.h"
#include "../enums/ScalarType.h"
#include "../enums/UniformType.h"
#include "../enums/VertexAttribIType.h"
#include "../enums/VertexAttribPointerType.h"
#include "../enums/VertexAttribType.h"
#include "../enums/WeightPointerTypeARB.h"

namespace gl::oes::depth_texture {
using gl::group::attribute_type;
using gl::group::color_pointer_type;
using gl::group::depth_stencil_texture_mode;
using gl::group::draw_elements_type;
using gl::group::element_pointer_type_ati;
using gl::group::internal_format;
using gl::group::list_name_type;
using gl::group::matrix_index_pointer_type_arb;
using gl::group::pixel_format;
using gl::group::pixel_type;
using gl::group::replacement_code_type_sun;
using gl::group::scalar_type;
using gl::group::uniform_type;
using gl::group::vertex_attrib_int;
using gl::group::vertex_attrib_pointer_type;
using gl::group::vertex_attrib_type;
using gl::group::weight_pointer_type_arb;

namespace values {
} // namespace values
} // namespace gl::oes::depth_texture
#endif // GL_OES_depth_texture
namespace gl::oes::depth_texture {
constexpr auto name = "GL_OES_depth_texture";
}
