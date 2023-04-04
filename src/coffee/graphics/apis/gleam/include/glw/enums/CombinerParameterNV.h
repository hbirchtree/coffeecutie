#pragma once

#include "common.h"

namespace gl::group {

// CombinerParameterNV
enum class combiner_parameter_nv : u32
{
#ifdef GL_COMBINER_COMPONENT_USAGE_NV
    combiner_component_usage_nv = GL_COMBINER_COMPONENT_USAGE_NV,
#endif
#ifdef GL_COMBINER_INPUT_NV
    combiner_input_nv = GL_COMBINER_INPUT_NV,
#endif
#ifdef GL_COMBINER_MAPPING_NV
    combiner_mapping_nv = GL_COMBINER_MAPPING_NV,
#endif
}; // enum class combiner_parameter_nv

} // namespace gl::group
