#pragma once

#ifdef GL_OES_texture_float
#include "../enums/AttributeType.h"
#include "../enums/BinormalPointerTypeEXT.h"
#include "../enums/FogCoordinatePointerType.h"
#include "../enums/FogPointerTypeEXT.h"
#include "../enums/FogPointerTypeIBM.h"
#include "../enums/IndexPointerType.h"
#include "../enums/ListNameType.h"
#include "../enums/MapTypeNV.h"
#include "../enums/NormalPointerType.h"
#include "../enums/PixelType.h"
#include "../enums/SecondaryColorPointerTypeIBM.h"
#include "../enums/TangentPointerTypeEXT.h"
#include "../enums/TexCoordPointerType.h"
#include "../enums/UniformType.h"
#include "../enums/VertexAttribPointerType.h"
#include "../enums/VertexAttribType.h"
#include "../enums/VertexPointerType.h"
#include "../enums/VertexWeightPointerTypeEXT.h"
#include "../enums/WeightPointerTypeARB.h"

namespace gl::oes::texture_float {
using gl::group::attribute_type;
using gl::group::binormal_pointer_type_ext;
using gl::group::fog_coordinate_pointer_type;
using gl::group::fog_pointer_type_ext;
using gl::group::fog_pointer_type_ibm;
using gl::group::index_pointer_type;
using gl::group::list_name_type;
using gl::group::map_type_nv;
using gl::group::normal_pointer_type;
using gl::group::pixel_type;
using gl::group::secondary_color_pointer_type_ibm;
using gl::group::tangent_pointer_type_ext;
using gl::group::tex_coord_pointer_type;
using gl::group::uniform_type;
using gl::group::vertex_attrib_pointer_type;
using gl::group::vertex_attrib_type;
using gl::group::vertex_pointer_type;
using gl::group::vertex_weight_pointer_type_ext;
using gl::group::weight_pointer_type_arb;

namespace values {
} // namespace values
} // namespace gl::oes::texture_float
#endif // GL_OES_texture_float
namespace gl::oes::texture_float {
constexpr auto name = "GL_OES_texture_float";
}
