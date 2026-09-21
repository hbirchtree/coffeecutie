#pragma once

#include "common.h"

namespace gl::group {

// SamplePatternSGIS
enum class sample_pattern_sgis : u32
{
    n1pass_ext    = 0x80A1, // GL_1PASS_EXT
    n1pass_sgis   = 0x80A1, // GL_1PASS_SGIS
    n2pass_0_ext  = 0x80A2, // GL_2PASS_0_EXT
    n2pass_0_sgis = 0x80A2, // GL_2PASS_0_SGIS
    n2pass_1_ext  = 0x80A3, // GL_2PASS_1_EXT
    n2pass_1_sgis = 0x80A3, // GL_2PASS_1_SGIS
    n4pass_0_ext  = 0x80A4, // GL_4PASS_0_EXT
    n4pass_0_sgis = 0x80A4, // GL_4PASS_0_SGIS
    n4pass_1_ext  = 0x80A5, // GL_4PASS_1_EXT
    n4pass_1_sgis = 0x80A5, // GL_4PASS_1_SGIS
    n4pass_2_ext  = 0x80A6, // GL_4PASS_2_EXT
    n4pass_2_sgis = 0x80A6, // GL_4PASS_2_SGIS
    n4pass_3_ext  = 0x80A7, // GL_4PASS_3_EXT
    n4pass_3_sgis = 0x80A7, // GL_4PASS_3_SGIS
}; // enum class sample_pattern_sgis

} // namespace gl::group
