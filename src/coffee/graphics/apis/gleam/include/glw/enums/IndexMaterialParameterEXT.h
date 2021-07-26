#pragma once

#include "common.h"

namespace gl::group {

// IndexMaterialParameterEXT
enum class index_material_parameter_ext : ::libc_types::u32
{
#ifdef GL_INDEX_OFFSET
    index_offset = GL_INDEX_OFFSET,
#endif
}; // enum class index_material_parameter_ext

} // namespace gl::group
