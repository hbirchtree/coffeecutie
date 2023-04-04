#pragma once

#include "common.h"

namespace gl::group {

// GetVariantValueEXT
enum class get_variant_value_ext : u32
{
#ifdef GL_VARIANT_ARRAY_STRIDE_EXT
    variant_array_stride_ext = GL_VARIANT_ARRAY_STRIDE_EXT,
#endif
#ifdef GL_VARIANT_ARRAY_TYPE_EXT
    variant_array_type_ext = GL_VARIANT_ARRAY_TYPE_EXT,
#endif
#ifdef GL_VARIANT_DATATYPE_EXT
    variant_datatype_ext = GL_VARIANT_DATATYPE_EXT,
#endif
#ifdef GL_VARIANT_VALUE_EXT
    variant_value_ext = GL_VARIANT_VALUE_EXT,
#endif
}; // enum class get_variant_value_ext

} // namespace gl::group
