#pragma once

#include "common.h"

namespace gl::group {

// PathTransformType
enum class path_transform_type : ::libc_types::u32
{
#ifdef GL_AFFINE_2D_NV
    affine_2d_nv = GL_AFFINE_2D_NV,
#endif
#ifdef GL_AFFINE_3D_NV
    affine_3d_nv = GL_AFFINE_3D_NV,
#endif
#ifdef GL_NONE
    none = GL_NONE,
#endif
#ifdef GL_TRANSLATE_2D_NV
    translate_2d_nv = GL_TRANSLATE_2D_NV,
#endif
#ifdef GL_TRANSLATE_3D_NV
    translate_3d_nv = GL_TRANSLATE_3D_NV,
#endif
#ifdef GL_TRANSLATE_X_NV
    translate_x_nv = GL_TRANSLATE_X_NV,
#endif
#ifdef GL_TRANSLATE_Y_NV
    translate_y_nv = GL_TRANSLATE_Y_NV,
#endif
#ifdef GL_TRANSPOSE_AFFINE_2D_NV
    transpose_affine_2d_nv = GL_TRANSPOSE_AFFINE_2D_NV,
#endif
#ifdef GL_TRANSPOSE_AFFINE_3D_NV
    transpose_affine_3d_nv = GL_TRANSPOSE_AFFINE_3D_NV,
#endif
}; // enum class path_transform_type

} // namespace gl::group
