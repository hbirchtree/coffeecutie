#pragma once

#include "common.h"

namespace gl::group {

// SamplePatternEXT
enum class sample_pattern_ext : ::libc_types::u32
{
#ifdef GL_1PASS_EXT
    1pass_ext = GL_1PASS_EXT,
#endif
#ifdef GL_2PASS_0_EXT
    2pass_0_ext = GL_2PASS_0_EXT,
#endif
#ifdef GL_2PASS_1_EXT
    2pass_1_ext = GL_2PASS_1_EXT,
#endif
#ifdef GL_4PASS_0_EXT
    4pass_0_ext = GL_4PASS_0_EXT,
#endif
#ifdef GL_4PASS_1_EXT
    4pass_1_ext = GL_4PASS_1_EXT,
#endif
#ifdef GL_4PASS_2_EXT
    4pass_2_ext = GL_4PASS_2_EXT,
#endif
#ifdef GL_4PASS_3_EXT
    4pass_3_ext = GL_4PASS_3_EXT,
#endif
}; // enum class sample_pattern_ext

} // namespace gl::group
