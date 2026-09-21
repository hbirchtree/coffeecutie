#pragma once

#include "common.h"

namespace gl::group {

// PathTransformType
enum class path_transform_type : u32
{
    none                   = 0,      // GL_NONE
    affine_2d_nv           = 0x9092, // GL_AFFINE_2D_NV
    affine_3d_nv           = 0x9094, // GL_AFFINE_3D_NV
    translate_2d_nv        = 0x9090, // GL_TRANSLATE_2D_NV
    translate_3d_nv        = 0x9091, // GL_TRANSLATE_3D_NV
    translate_x_nv         = 0x908E, // GL_TRANSLATE_X_NV
    translate_y_nv         = 0x908F, // GL_TRANSLATE_Y_NV
    transpose_affine_2d_nv = 0x9096, // GL_TRANSPOSE_AFFINE_2D_NV
    transpose_affine_3d_nv = 0x9098, // GL_TRANSPOSE_AFFINE_3D_NV
}; // enum class path_transform_type

} // namespace gl::group
