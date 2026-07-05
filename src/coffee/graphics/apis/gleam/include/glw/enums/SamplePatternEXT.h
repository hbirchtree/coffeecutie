#pragma once

#include "common.h"

namespace gl::group {

// SamplePatternEXT
enum class sample_pattern_ext : u32
{
#ifdef GL_1PASS_EXT
    n1pass_ext = GL_1PASS_EXT,
#endif
#ifdef GL_2PASS_0_EXT
    n2pass_0_ext = GL_2PASS_0_EXT,
#endif
#ifdef GL_2PASS_1_EXT
    n2pass_1_ext = GL_2PASS_1_EXT,
#endif
#ifdef GL_4PASS_0_EXT
    n4pass_0_ext = GL_4PASS_0_EXT,
#endif
#ifdef GL_4PASS_1_EXT
    n4pass_1_ext = GL_4PASS_1_EXT,
#endif
#ifdef GL_4PASS_2_EXT
    n4pass_2_ext = GL_4PASS_2_EXT,
#endif
#ifdef GL_4PASS_3_EXT
    n4pass_3_ext = GL_4PASS_3_EXT,
#endif
}; // enum class sample_pattern_ext

} // namespace gl::group
