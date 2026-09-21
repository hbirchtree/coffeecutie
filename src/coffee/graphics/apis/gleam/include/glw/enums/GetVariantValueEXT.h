#pragma once

#include "common.h"

namespace gl::group {

// GetVariantValueEXT
enum class get_variant_value_ext : u32
{
    variant_array_stride_ext = 0x87E6, // GL_VARIANT_ARRAY_STRIDE_EXT
    variant_array_type_ext   = 0x87E7, // GL_VARIANT_ARRAY_TYPE_EXT
    variant_datatype_ext     = 0x87E5, // GL_VARIANT_DATATYPE_EXT
    variant_value_ext        = 0x87E4, // GL_VARIANT_VALUE_EXT
}; // enum class get_variant_value_ext

} // namespace gl::group
