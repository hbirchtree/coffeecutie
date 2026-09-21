#pragma once

#include "common.h"

namespace gl::group {

// UniformType
enum class uniform_type : u32
{
    bool_                      = 0x8B56, // GL_BOOL
    bool_vec2                  = 0x8B57, // GL_BOOL_VEC2
    bool_vec3                  = 0x8B58, // GL_BOOL_VEC3
    bool_vec4                  = 0x8B59, // GL_BOOL_VEC4
    double_                    = 0x140A, // GL_DOUBLE
    double_mat2                = 0x8F46, // GL_DOUBLE_MAT2
    double_mat2x3              = 0x8F49, // GL_DOUBLE_MAT2x3
    double_mat2x4              = 0x8F4A, // GL_DOUBLE_MAT2x4
    double_mat3                = 0x8F47, // GL_DOUBLE_MAT3
    double_mat3x2              = 0x8F4B, // GL_DOUBLE_MAT3x2
    double_mat3x4              = 0x8F4C, // GL_DOUBLE_MAT3x4
    double_mat4                = 0x8F48, // GL_DOUBLE_MAT4
    double_mat4x2              = 0x8F4D, // GL_DOUBLE_MAT4x2
    double_mat4x3              = 0x8F4E, // GL_DOUBLE_MAT4x3
    double_vec2                = 0x8FFC, // GL_DOUBLE_VEC2
    double_vec3                = 0x8FFD, // GL_DOUBLE_VEC3
    double_vec4                = 0x8FFE, // GL_DOUBLE_VEC4
    float_                     = 0x1406, // GL_FLOAT
    float_mat2                 = 0x8B5A, // GL_FLOAT_MAT2
    float_mat2x3               = 0x8B65, // GL_FLOAT_MAT2x3
    float_mat2x4               = 0x8B66, // GL_FLOAT_MAT2x4
    float_mat3                 = 0x8B5B, // GL_FLOAT_MAT3
    float_mat3x2               = 0x8B67, // GL_FLOAT_MAT3x2
    float_mat3x4               = 0x8B68, // GL_FLOAT_MAT3x4
    float_mat4                 = 0x8B5C, // GL_FLOAT_MAT4
    float_mat4x2               = 0x8B69, // GL_FLOAT_MAT4x2
    float_mat4x3               = 0x8B6A, // GL_FLOAT_MAT4x3
    float_vec2                 = 0x8B50, // GL_FLOAT_VEC2
    float_vec3                 = 0x8B51, // GL_FLOAT_VEC3
    float_vec4                 = 0x8B52, // GL_FLOAT_VEC4
    int_                       = 0x1404, // GL_INT
    int_sampler_1d             = 0x8DC9, // GL_INT_SAMPLER_1D
    int_sampler_1d_array       = 0x8DCE, // GL_INT_SAMPLER_1D_ARRAY
    int_sampler_2d             = 0x8DCA, // GL_INT_SAMPLER_2D
    int_sampler_2d_array       = 0x8DCF, // GL_INT_SAMPLER_2D_ARRAY
    int_sampler_2d_multisample = 0x9109, // GL_INT_SAMPLER_2D_MULTISAMPLE
    int_sampler_2d_multisample_array =
        0x910C, // GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
    int_sampler_2d_rect           = 0x8DCD, // GL_INT_SAMPLER_2D_RECT
    int_sampler_3d                = 0x8DCB, // GL_INT_SAMPLER_3D
    int_sampler_buffer            = 0x8DD0, // GL_INT_SAMPLER_BUFFER
    int_sampler_cube              = 0x8DCC, // GL_INT_SAMPLER_CUBE
    int_sampler_cube_map_array    = 0x900E, // GL_INT_SAMPLER_CUBE_MAP_ARRAY
    int_vec2                      = 0x8B53, // GL_INT_VEC2
    int_vec3                      = 0x8B54, // GL_INT_VEC3
    int_vec4                      = 0x8B55, // GL_INT_VEC4
    sampler_1d                    = 0x8B5D, // GL_SAMPLER_1D
    sampler_1d_array              = 0x8DC0, // GL_SAMPLER_1D_ARRAY
    sampler_1d_array_shadow       = 0x8DC3, // GL_SAMPLER_1D_ARRAY_SHADOW
    sampler_1d_shadow             = 0x8B61, // GL_SAMPLER_1D_SHADOW
    sampler_2d                    = 0x8B5E, // GL_SAMPLER_2D
    sampler_2d_array              = 0x8DC1, // GL_SAMPLER_2D_ARRAY
    sampler_2d_array_shadow       = 0x8DC4, // GL_SAMPLER_2D_ARRAY_SHADOW
    sampler_2d_multisample        = 0x9108, // GL_SAMPLER_2D_MULTISAMPLE
    sampler_2d_multisample_array  = 0x910B, // GL_SAMPLER_2D_MULTISAMPLE_ARRAY
    sampler_2d_rect               = 0x8B63, // GL_SAMPLER_2D_RECT
    sampler_2d_rect_shadow        = 0x8B64, // GL_SAMPLER_2D_RECT_SHADOW
    sampler_2d_shadow             = 0x8B62, // GL_SAMPLER_2D_SHADOW
    sampler_3d                    = 0x8B5F, // GL_SAMPLER_3D
    sampler_buffer                = 0x8DC2, // GL_SAMPLER_BUFFER
    sampler_cube                  = 0x8B60, // GL_SAMPLER_CUBE
    sampler_cube_map_array        = 0x900C, // GL_SAMPLER_CUBE_MAP_ARRAY
    sampler_cube_map_array_shadow = 0x900D, // GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW
    sampler_cube_shadow           = 0x8DC5, // GL_SAMPLER_CUBE_SHADOW
    unsigned_int                  = 0x1405, // GL_UNSIGNED_INT
    unsigned_int_sampler_1d       = 0x8DD1, // GL_UNSIGNED_INT_SAMPLER_1D
    unsigned_int_sampler_1d_array = 0x8DD6, // GL_UNSIGNED_INT_SAMPLER_1D_ARRAY
    unsigned_int_sampler_2d       = 0x8DD2, // GL_UNSIGNED_INT_SAMPLER_2D
    unsigned_int_sampler_2d_array = 0x8DD7, // GL_UNSIGNED_INT_SAMPLER_2D_ARRAY
    unsigned_int_sampler_2d_multisample =
        0x910A, // GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE
    unsigned_int_sampler_2d_multisample_array =
        0x910D, // GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
    unsigned_int_sampler_2d_rect = 0x8DD5, // GL_UNSIGNED_INT_SAMPLER_2D_RECT
    unsigned_int_sampler_3d      = 0x8DD3, // GL_UNSIGNED_INT_SAMPLER_3D
    unsigned_int_sampler_buffer  = 0x8DD8, // GL_UNSIGNED_INT_SAMPLER_BUFFER
    unsigned_int_sampler_cube    = 0x8DD4, // GL_UNSIGNED_INT_SAMPLER_CUBE
    unsigned_int_sampler_cube_map_array =
        0x900F,                 // GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY
    unsigned_int_vec2 = 0x8DC6, // GL_UNSIGNED_INT_VEC2
    unsigned_int_vec3 = 0x8DC7, // GL_UNSIGNED_INT_VEC3
    unsigned_int_vec4 = 0x8DC8, // GL_UNSIGNED_INT_VEC4
}; // enum class uniform_type

} // namespace gl::group
