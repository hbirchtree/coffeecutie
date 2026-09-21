#pragma once

#include "common.h"

namespace gl::group {

// AttributeType
enum class attribute_type : u32
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
    image_1d                   = 0x904C, // GL_IMAGE_1D
    image_1d_array             = 0x9052, // GL_IMAGE_1D_ARRAY
    image_2d                   = 0x904D, // GL_IMAGE_2D
    image_2d_array             = 0x9053, // GL_IMAGE_2D_ARRAY
    image_2d_multisample       = 0x9055, // GL_IMAGE_2D_MULTISAMPLE
    image_2d_multisample_array = 0x9056, // GL_IMAGE_2D_MULTISAMPLE_ARRAY
    image_2d_rect              = 0x904F, // GL_IMAGE_2D_RECT
    image_3d                   = 0x904E, // GL_IMAGE_3D
    image_buffer               = 0x9051, // GL_IMAGE_BUFFER
    image_cube                 = 0x9050, // GL_IMAGE_CUBE
    image_cube_map_array       = 0x9054, // GL_IMAGE_CUBE_MAP_ARRAY
    int_                       = 0x1404, // GL_INT
    int_image_1d               = 0x9057, // GL_INT_IMAGE_1D
    int_image_1d_array         = 0x905D, // GL_INT_IMAGE_1D_ARRAY
    int_image_2d               = 0x9058, // GL_INT_IMAGE_2D
    int_image_2d_array         = 0x905E, // GL_INT_IMAGE_2D_ARRAY
    int_image_2d_multisample   = 0x9060, // GL_INT_IMAGE_2D_MULTISAMPLE
    int_image_2d_multisample_array =
        0x9061,                          // GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY
    int_image_2d_rect          = 0x905A, // GL_INT_IMAGE_2D_RECT
    int_image_3d               = 0x9059, // GL_INT_IMAGE_3D
    int_image_buffer           = 0x905C, // GL_INT_IMAGE_BUFFER
    int_image_cube             = 0x905B, // GL_INT_IMAGE_CUBE
    int_image_cube_map_array   = 0x905F, // GL_INT_IMAGE_CUBE_MAP_ARRAY
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
    sampler_1d_array_shadow       = 0x8DC3, // GL_SAMPLER_1D_ARRAY_SHADOW
    sampler_1d_shadow             = 0x8B61, // GL_SAMPLER_1D_SHADOW
    sampler_2d                    = 0x8B5E, // GL_SAMPLER_2D
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
    unsigned_int_image_1d         = 0x9062, // GL_UNSIGNED_INT_IMAGE_1D
    unsigned_int_image_1d_array   = 0x9068, // GL_UNSIGNED_INT_IMAGE_1D_ARRAY
    unsigned_int_image_2d         = 0x9063, // GL_UNSIGNED_INT_IMAGE_2D
    unsigned_int_image_2d_array   = 0x9069, // GL_UNSIGNED_INT_IMAGE_2D_ARRAY
    unsigned_int_image_2d_multisample =
        0x906B, // GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE
    unsigned_int_image_2d_multisample_array =
        0x906C, // GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY
    unsigned_int_image_2d_rect = 0x9065, // GL_UNSIGNED_INT_IMAGE_2D_RECT
    unsigned_int_image_3d      = 0x9064, // GL_UNSIGNED_INT_IMAGE_3D
    unsigned_int_image_buffer  = 0x9067, // GL_UNSIGNED_INT_IMAGE_BUFFER
    unsigned_int_image_cube    = 0x9066, // GL_UNSIGNED_INT_IMAGE_CUBE
    unsigned_int_image_cube_map_array =
        0x906A, // GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY
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
        0x900F, // GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY
    unsigned_int_vec2          = 0x8DC6, // GL_UNSIGNED_INT_VEC2
    unsigned_int_vec3          = 0x8DC7, // GL_UNSIGNED_INT_VEC3
    unsigned_int_vec4          = 0x8DC8, // GL_UNSIGNED_INT_VEC4
    bool_arb                   = 0x8B56, // GL_BOOL_ARB
    bool_vec2_arb              = 0x8B57, // GL_BOOL_VEC2_ARB
    bool_vec3_arb              = 0x8B58, // GL_BOOL_VEC3_ARB
    bool_vec4_arb              = 0x8B59, // GL_BOOL_VEC4_ARB
    float_mat2_arb             = 0x8B5A, // GL_FLOAT_MAT2_ARB
    float_mat2x3_nv            = 0x8B65, // GL_FLOAT_MAT2x3_NV
    float_mat2x4_nv            = 0x8B66, // GL_FLOAT_MAT2x4_NV
    float_mat3_arb             = 0x8B5B, // GL_FLOAT_MAT3_ARB
    float_mat3x2_nv            = 0x8B67, // GL_FLOAT_MAT3x2_NV
    float_mat3x4_nv            = 0x8B68, // GL_FLOAT_MAT3x4_NV
    float_mat4_arb             = 0x8B5C, // GL_FLOAT_MAT4_ARB
    float_mat4x2_nv            = 0x8B69, // GL_FLOAT_MAT4x2_NV
    float_mat4x3_nv            = 0x8B6A, // GL_FLOAT_MAT4x3_NV
    float_vec2_arb             = 0x8B50, // GL_FLOAT_VEC2_ARB
    float_vec3_arb             = 0x8B51, // GL_FLOAT_VEC3_ARB
    float_vec4_arb             = 0x8B52, // GL_FLOAT_VEC4_ARB
    int64_arb                  = 0x140E, // GL_INT64_ARB
    int64_nv                   = 0x140E, // GL_INT64_NV
    int64_vec2_arb             = 0x8FE9, // GL_INT64_VEC2_ARB
    int64_vec3_arb             = 0x8FEA, // GL_INT64_VEC3_ARB
    int64_vec4_arb             = 0x8FEB, // GL_INT64_VEC4_ARB
    int_vec2_arb               = 0x8B53, // GL_INT_VEC2_ARB
    int_vec3_arb               = 0x8B54, // GL_INT_VEC3_ARB
    int_vec4_arb               = 0x8B55, // GL_INT_VEC4_ARB
    sampler_1d_arb             = 0x8B5D, // GL_SAMPLER_1D_ARB
    sampler_1d_shadow_arb      = 0x8B61, // GL_SAMPLER_1D_SHADOW_ARB
    sampler_2d_arb             = 0x8B5E, // GL_SAMPLER_2D_ARB
    sampler_2d_rect_arb        = 0x8B63, // GL_SAMPLER_2D_RECT_ARB
    sampler_2d_rect_shadow_arb = 0x8B64, // GL_SAMPLER_2D_RECT_SHADOW_ARB
    sampler_2d_shadow_arb      = 0x8B62, // GL_SAMPLER_2D_SHADOW_ARB
    sampler_2d_shadow_ext      = 0x8B62, // GL_SAMPLER_2D_SHADOW_EXT
    sampler_3d_arb             = 0x8B5F, // GL_SAMPLER_3D_ARB
    sampler_3d_oes             = 0x8B5F, // GL_SAMPLER_3D_OES
    sampler_cube_arb           = 0x8B60, // GL_SAMPLER_CUBE_ARB
    unsigned_int64_arb         = 0x140F, // GL_UNSIGNED_INT64_ARB
    unsigned_int64_nv          = 0x140F, // GL_UNSIGNED_INT64_NV
    unsigned_int64_vec2_arb    = 0x8FF5, // GL_UNSIGNED_INT64_VEC2_ARB
    unsigned_int64_vec3_arb    = 0x8FF6, // GL_UNSIGNED_INT64_VEC3_ARB
    unsigned_int64_vec4_arb    = 0x8FF7, // GL_UNSIGNED_INT64_VEC4_ARB
}; // enum class attribute_type

} // namespace gl::group
