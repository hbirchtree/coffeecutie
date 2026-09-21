#pragma once

#include "common.h"

namespace gl::group {

// CombinerParameterNV
enum class combiner_parameter_nv : u32
{
    combiner_component_usage_nv = 0x8544, // GL_COMBINER_COMPONENT_USAGE_NV
    combiner_input_nv           = 0x8542, // GL_COMBINER_INPUT_NV
    combiner_mapping_nv         = 0x8543, // GL_COMBINER_MAPPING_NV
}; // enum class combiner_parameter_nv

} // namespace gl::group
