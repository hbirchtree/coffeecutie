#pragma once

#include "common.h"

namespace gl::group {

// MapAttribParameterNV
enum class map_attrib_parameter_nv : ::libc_types::u32
{
#ifdef GL_MAP_ATTRIB_U_ORDER_NV
    map_attrib_u_order_nv = GL_MAP_ATTRIB_U_ORDER_NV,
#endif
#ifdef GL_MAP_ATTRIB_V_ORDER_NV
    map_attrib_v_order_nv = GL_MAP_ATTRIB_V_ORDER_NV,
#endif
}; // enum class map_attrib_parameter_nv

} // namespace gl::group
