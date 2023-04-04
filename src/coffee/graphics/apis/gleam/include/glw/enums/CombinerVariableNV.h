#pragma once

#include "common.h"

namespace gl::group {

// CombinerVariableNV
enum class combiner_variable_nv : u32
{
#ifdef GL_VARIABLE_A_NV
    variable_a_nv = GL_VARIABLE_A_NV,
#endif
#ifdef GL_VARIABLE_B_NV
    variable_b_nv = GL_VARIABLE_B_NV,
#endif
#ifdef GL_VARIABLE_C_NV
    variable_c_nv = GL_VARIABLE_C_NV,
#endif
#ifdef GL_VARIABLE_D_NV
    variable_d_nv = GL_VARIABLE_D_NV,
#endif
#ifdef GL_VARIABLE_E_NV
    variable_e_nv = GL_VARIABLE_E_NV,
#endif
#ifdef GL_VARIABLE_F_NV
    variable_f_nv = GL_VARIABLE_F_NV,
#endif
#ifdef GL_VARIABLE_G_NV
    variable_g_nv = GL_VARIABLE_G_NV,
#endif
}; // enum class combiner_variable_nv

} // namespace gl::group
