#pragma once

#include "common.h"

namespace gl::group {

// AttributeType
enum class attribute_type : ::libc_types::u32
{
#ifdef GL_BOOL
    bool_ = GL_BOOL,
#endif
#ifdef GL_BOOL_ARB
    bool_arb = GL_BOOL_ARB,
#endif
#ifdef GL_BOOL_VEC2
    bool_vec2 = GL_BOOL_VEC2,
#endif
#ifdef GL_BOOL_VEC2_ARB
    bool_vec2_arb = GL_BOOL_VEC2_ARB,
#endif
#ifdef GL_BOOL_VEC3
    bool_vec3 = GL_BOOL_VEC3,
#endif
#ifdef GL_BOOL_VEC3_ARB
    bool_vec3_arb = GL_BOOL_VEC3_ARB,
#endif
#ifdef GL_BOOL_VEC4
    bool_vec4 = GL_BOOL_VEC4,
#endif
#ifdef GL_BOOL_VEC4_ARB
    bool_vec4_arb = GL_BOOL_VEC4_ARB,
#endif
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
#ifdef GL_DOUBLE_MAT2
    double_mat2 = GL_DOUBLE_MAT2,
#endif
#ifdef GL_DOUBLE_MAT2x3
    double_mat2x3 = GL_DOUBLE_MAT2x3,
#endif
#ifdef GL_DOUBLE_MAT2x4
    double_mat2x4 = GL_DOUBLE_MAT2x4,
#endif
#ifdef GL_DOUBLE_MAT3
    double_mat3 = GL_DOUBLE_MAT3,
#endif
#ifdef GL_DOUBLE_MAT3x2
    double_mat3x2 = GL_DOUBLE_MAT3x2,
#endif
#ifdef GL_DOUBLE_MAT3x4
    double_mat3x4 = GL_DOUBLE_MAT3x4,
#endif
#ifdef GL_DOUBLE_MAT4
    double_mat4 = GL_DOUBLE_MAT4,
#endif
#ifdef GL_DOUBLE_MAT4x2
    double_mat4x2 = GL_DOUBLE_MAT4x2,
#endif
#ifdef GL_DOUBLE_MAT4x3
    double_mat4x3 = GL_DOUBLE_MAT4x3,
#endif
#ifdef GL_DOUBLE_VEC2
    double_vec2 = GL_DOUBLE_VEC2,
#endif
#ifdef GL_DOUBLE_VEC3
    double_vec3 = GL_DOUBLE_VEC3,
#endif
#ifdef GL_DOUBLE_VEC4
    double_vec4 = GL_DOUBLE_VEC4,
#endif
#ifdef GL_FLOAT
    float_ = GL_FLOAT,
#endif
#ifdef GL_FLOAT_MAT2
    float_mat2 = GL_FLOAT_MAT2,
#endif
#ifdef GL_FLOAT_MAT2_ARB
    float_mat2_arb = GL_FLOAT_MAT2_ARB,
#endif
#ifdef GL_FLOAT_MAT2x3
    float_mat2x3 = GL_FLOAT_MAT2x3,
#endif
#ifdef GL_FLOAT_MAT2x3_NV
    float_mat2x3_nv = GL_FLOAT_MAT2x3_NV,
#endif
#ifdef GL_FLOAT_MAT2x4
    float_mat2x4 = GL_FLOAT_MAT2x4,
#endif
#ifdef GL_FLOAT_MAT2x4_NV
    float_mat2x4_nv = GL_FLOAT_MAT2x4_NV,
#endif
#ifdef GL_FLOAT_MAT3
    float_mat3 = GL_FLOAT_MAT3,
#endif
#ifdef GL_FLOAT_MAT3_ARB
    float_mat3_arb = GL_FLOAT_MAT3_ARB,
#endif
#ifdef GL_FLOAT_MAT3x2
    float_mat3x2 = GL_FLOAT_MAT3x2,
#endif
#ifdef GL_FLOAT_MAT3x2_NV
    float_mat3x2_nv = GL_FLOAT_MAT3x2_NV,
#endif
#ifdef GL_FLOAT_MAT3x4
    float_mat3x4 = GL_FLOAT_MAT3x4,
#endif
#ifdef GL_FLOAT_MAT3x4_NV
    float_mat3x4_nv = GL_FLOAT_MAT3x4_NV,
#endif
#ifdef GL_FLOAT_MAT4
    float_mat4 = GL_FLOAT_MAT4,
#endif
#ifdef GL_FLOAT_MAT4_ARB
    float_mat4_arb = GL_FLOAT_MAT4_ARB,
#endif
#ifdef GL_FLOAT_MAT4x2
    float_mat4x2 = GL_FLOAT_MAT4x2,
#endif
#ifdef GL_FLOAT_MAT4x2_NV
    float_mat4x2_nv = GL_FLOAT_MAT4x2_NV,
#endif
#ifdef GL_FLOAT_MAT4x3
    float_mat4x3 = GL_FLOAT_MAT4x3,
#endif
#ifdef GL_FLOAT_MAT4x3_NV
    float_mat4x3_nv = GL_FLOAT_MAT4x3_NV,
#endif
#ifdef GL_FLOAT_VEC2
    float_vec2 = GL_FLOAT_VEC2,
#endif
#ifdef GL_FLOAT_VEC2_ARB
    float_vec2_arb = GL_FLOAT_VEC2_ARB,
#endif
#ifdef GL_FLOAT_VEC3
    float_vec3 = GL_FLOAT_VEC3,
#endif
#ifdef GL_FLOAT_VEC3_ARB
    float_vec3_arb = GL_FLOAT_VEC3_ARB,
#endif
#ifdef GL_FLOAT_VEC4
    float_vec4 = GL_FLOAT_VEC4,
#endif
#ifdef GL_FLOAT_VEC4_ARB
    float_vec4_arb = GL_FLOAT_VEC4_ARB,
#endif
#ifdef GL_IMAGE_1D
    image_1d = GL_IMAGE_1D,
#endif
#ifdef GL_IMAGE_1D_ARRAY
    image_1d_array = GL_IMAGE_1D_ARRAY,
#endif
#ifdef GL_IMAGE_2D
    image_2d = GL_IMAGE_2D,
#endif
#ifdef GL_IMAGE_2D_ARRAY
    image_2d_array = GL_IMAGE_2D_ARRAY,
#endif
#ifdef GL_IMAGE_2D_MULTISAMPLE
    image_2d_multisample = GL_IMAGE_2D_MULTISAMPLE,
#endif
#ifdef GL_IMAGE_2D_MULTISAMPLE_ARRAY
    image_2d_multisample_array = GL_IMAGE_2D_MULTISAMPLE_ARRAY,
#endif
#ifdef GL_IMAGE_2D_RECT
    image_2d_rect = GL_IMAGE_2D_RECT,
#endif
#ifdef GL_IMAGE_3D
    image_3d = GL_IMAGE_3D,
#endif
#ifdef GL_IMAGE_BUFFER
    image_buffer = GL_IMAGE_BUFFER,
#endif
#ifdef GL_IMAGE_CUBE
    image_cube = GL_IMAGE_CUBE,
#endif
#ifdef GL_IMAGE_CUBE_MAP_ARRAY
    image_cube_map_array = GL_IMAGE_CUBE_MAP_ARRAY,
#endif
#ifdef GL_INT
    int_ = GL_INT,
#endif
#ifdef GL_INT64_ARB
    int64_arb = GL_INT64_ARB,
#endif
#ifdef GL_INT64_NV
    int64_nv = GL_INT64_NV,
#endif
#ifdef GL_INT64_VEC2_ARB
    int64_vec2_arb = GL_INT64_VEC2_ARB,
#endif
#ifdef GL_INT64_VEC3_ARB
    int64_vec3_arb = GL_INT64_VEC3_ARB,
#endif
#ifdef GL_INT64_VEC4_ARB
    int64_vec4_arb = GL_INT64_VEC4_ARB,
#endif
#ifdef GL_INT_IMAGE_1D
    int_image_1d = GL_INT_IMAGE_1D,
#endif
#ifdef GL_INT_IMAGE_1D_ARRAY
    int_image_1d_array = GL_INT_IMAGE_1D_ARRAY,
#endif
#ifdef GL_INT_IMAGE_2D
    int_image_2d = GL_INT_IMAGE_2D,
#endif
#ifdef GL_INT_IMAGE_2D_ARRAY
    int_image_2d_array = GL_INT_IMAGE_2D_ARRAY,
#endif
#ifdef GL_INT_IMAGE_2D_MULTISAMPLE
    int_image_2d_multisample = GL_INT_IMAGE_2D_MULTISAMPLE,
#endif
#ifdef GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY
    int_image_2d_multisample_array = GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY,
#endif
#ifdef GL_INT_IMAGE_2D_RECT
    int_image_2d_rect = GL_INT_IMAGE_2D_RECT,
#endif
#ifdef GL_INT_IMAGE_3D
    int_image_3d = GL_INT_IMAGE_3D,
#endif
#ifdef GL_INT_IMAGE_BUFFER
    int_image_buffer = GL_INT_IMAGE_BUFFER,
#endif
#ifdef GL_INT_IMAGE_CUBE
    int_image_cube = GL_INT_IMAGE_CUBE,
#endif
#ifdef GL_INT_IMAGE_CUBE_MAP_ARRAY
    int_image_cube_map_array = GL_INT_IMAGE_CUBE_MAP_ARRAY,
#endif
#ifdef GL_INT_SAMPLER_1D
    int_sampler_1d = GL_INT_SAMPLER_1D,
#endif
#ifdef GL_INT_SAMPLER_1D_ARRAY
    int_sampler_1d_array = GL_INT_SAMPLER_1D_ARRAY,
#endif
#ifdef GL_INT_SAMPLER_2D
    int_sampler_2d = GL_INT_SAMPLER_2D,
#endif
#ifdef GL_INT_SAMPLER_2D_ARRAY
    int_sampler_2d_array = GL_INT_SAMPLER_2D_ARRAY,
#endif
#ifdef GL_INT_SAMPLER_2D_MULTISAMPLE
    int_sampler_2d_multisample = GL_INT_SAMPLER_2D_MULTISAMPLE,
#endif
#ifdef GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
    int_sampler_2d_multisample_array = GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
#endif
#ifdef GL_INT_SAMPLER_2D_RECT
    int_sampler_2d_rect = GL_INT_SAMPLER_2D_RECT,
#endif
#ifdef GL_INT_SAMPLER_3D
    int_sampler_3d = GL_INT_SAMPLER_3D,
#endif
#ifdef GL_INT_SAMPLER_BUFFER
    int_sampler_buffer = GL_INT_SAMPLER_BUFFER,
#endif
#ifdef GL_INT_SAMPLER_CUBE
    int_sampler_cube = GL_INT_SAMPLER_CUBE,
#endif
#ifdef GL_INT_SAMPLER_CUBE_MAP_ARRAY
    int_sampler_cube_map_array = GL_INT_SAMPLER_CUBE_MAP_ARRAY,
#endif
#ifdef GL_INT_VEC2
    int_vec2 = GL_INT_VEC2,
#endif
#ifdef GL_INT_VEC2_ARB
    int_vec2_arb = GL_INT_VEC2_ARB,
#endif
#ifdef GL_INT_VEC3
    int_vec3 = GL_INT_VEC3,
#endif
#ifdef GL_INT_VEC3_ARB
    int_vec3_arb = GL_INT_VEC3_ARB,
#endif
#ifdef GL_INT_VEC4
    int_vec4 = GL_INT_VEC4,
#endif
#ifdef GL_INT_VEC4_ARB
    int_vec4_arb = GL_INT_VEC4_ARB,
#endif
#ifdef GL_SAMPLER_1D
    sampler_1d = GL_SAMPLER_1D,
#endif
#ifdef GL_SAMPLER_1D_ARB
    sampler_1d_arb = GL_SAMPLER_1D_ARB,
#endif
#ifdef GL_SAMPLER_1D_ARRAY_SHADOW
    sampler_1d_array_shadow = GL_SAMPLER_1D_ARRAY_SHADOW,
#endif
#ifdef GL_SAMPLER_1D_SHADOW
    sampler_1d_shadow = GL_SAMPLER_1D_SHADOW,
#endif
#ifdef GL_SAMPLER_1D_SHADOW_ARB
    sampler_1d_shadow_arb = GL_SAMPLER_1D_SHADOW_ARB,
#endif
#ifdef GL_SAMPLER_2D
    sampler_2d = GL_SAMPLER_2D,
#endif
#ifdef GL_SAMPLER_2D_ARB
    sampler_2d_arb = GL_SAMPLER_2D_ARB,
#endif
#ifdef GL_SAMPLER_2D_ARRAY_SHADOW
    sampler_2d_array_shadow = GL_SAMPLER_2D_ARRAY_SHADOW,
#endif
#ifdef GL_SAMPLER_2D_MULTISAMPLE
    sampler_2d_multisample = GL_SAMPLER_2D_MULTISAMPLE,
#endif
#ifdef GL_SAMPLER_2D_MULTISAMPLE_ARRAY
    sampler_2d_multisample_array = GL_SAMPLER_2D_MULTISAMPLE_ARRAY,
#endif
#ifdef GL_SAMPLER_2D_RECT
    sampler_2d_rect = GL_SAMPLER_2D_RECT,
#endif
#ifdef GL_SAMPLER_2D_RECT_ARB
    sampler_2d_rect_arb = GL_SAMPLER_2D_RECT_ARB,
#endif
#ifdef GL_SAMPLER_2D_RECT_SHADOW
    sampler_2d_rect_shadow = GL_SAMPLER_2D_RECT_SHADOW,
#endif
#ifdef GL_SAMPLER_2D_RECT_SHADOW_ARB
    sampler_2d_rect_shadow_arb = GL_SAMPLER_2D_RECT_SHADOW_ARB,
#endif
#ifdef GL_SAMPLER_2D_SHADOW
    sampler_2d_shadow = GL_SAMPLER_2D_SHADOW,
#endif
#ifdef GL_SAMPLER_2D_SHADOW_ARB
    sampler_2d_shadow_arb = GL_SAMPLER_2D_SHADOW_ARB,
#endif
#ifdef GL_SAMPLER_2D_SHADOW_EXT
    sampler_2d_shadow_ext = GL_SAMPLER_2D_SHADOW_EXT,
#endif
#ifdef GL_SAMPLER_3D
    sampler_3d = GL_SAMPLER_3D,
#endif
#ifdef GL_SAMPLER_3D_ARB
    sampler_3d_arb = GL_SAMPLER_3D_ARB,
#endif
#ifdef GL_SAMPLER_3D_OES
    sampler_3d_oes = GL_SAMPLER_3D_OES,
#endif
#ifdef GL_SAMPLER_BUFFER
    sampler_buffer = GL_SAMPLER_BUFFER,
#endif
#ifdef GL_SAMPLER_CUBE
    sampler_cube = GL_SAMPLER_CUBE,
#endif
#ifdef GL_SAMPLER_CUBE_ARB
    sampler_cube_arb = GL_SAMPLER_CUBE_ARB,
#endif
#ifdef GL_SAMPLER_CUBE_MAP_ARRAY
    sampler_cube_map_array = GL_SAMPLER_CUBE_MAP_ARRAY,
#endif
#ifdef GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW
    sampler_cube_map_array_shadow = GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW,
#endif
#ifdef GL_SAMPLER_CUBE_SHADOW
    sampler_cube_shadow = GL_SAMPLER_CUBE_SHADOW,
#endif
#ifdef GL_UNSIGNED_INT
    unsigned_int = GL_UNSIGNED_INT,
#endif
#ifdef GL_UNSIGNED_INT64_ARB
    unsigned_int64_arb = GL_UNSIGNED_INT64_ARB,
#endif
#ifdef GL_UNSIGNED_INT64_NV
    unsigned_int64_nv = GL_UNSIGNED_INT64_NV,
#endif
#ifdef GL_UNSIGNED_INT64_VEC2_ARB
    unsigned_int64_vec2_arb = GL_UNSIGNED_INT64_VEC2_ARB,
#endif
#ifdef GL_UNSIGNED_INT64_VEC3_ARB
    unsigned_int64_vec3_arb = GL_UNSIGNED_INT64_VEC3_ARB,
#endif
#ifdef GL_UNSIGNED_INT64_VEC4_ARB
    unsigned_int64_vec4_arb = GL_UNSIGNED_INT64_VEC4_ARB,
#endif
#ifdef GL_UNSIGNED_INT_IMAGE_1D
    unsigned_int_image_1d = GL_UNSIGNED_INT_IMAGE_1D,
#endif
#ifdef GL_UNSIGNED_INT_IMAGE_1D_ARRAY
    unsigned_int_image_1d_array = GL_UNSIGNED_INT_IMAGE_1D_ARRAY,
#endif
#ifdef GL_UNSIGNED_INT_IMAGE_2D
    unsigned_int_image_2d = GL_UNSIGNED_INT_IMAGE_2D,
#endif
#ifdef GL_UNSIGNED_INT_IMAGE_2D_ARRAY
    unsigned_int_image_2d_array = GL_UNSIGNED_INT_IMAGE_2D_ARRAY,
#endif
#ifdef GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE
    unsigned_int_image_2d_multisample = GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE,
#endif
#ifdef GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY
    unsigned_int_image_2d_multisample_array =
        GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY,
#endif
#ifdef GL_UNSIGNED_INT_IMAGE_2D_RECT
    unsigned_int_image_2d_rect = GL_UNSIGNED_INT_IMAGE_2D_RECT,
#endif
#ifdef GL_UNSIGNED_INT_IMAGE_3D
    unsigned_int_image_3d = GL_UNSIGNED_INT_IMAGE_3D,
#endif
#ifdef GL_UNSIGNED_INT_IMAGE_BUFFER
    unsigned_int_image_buffer = GL_UNSIGNED_INT_IMAGE_BUFFER,
#endif
#ifdef GL_UNSIGNED_INT_IMAGE_CUBE
    unsigned_int_image_cube = GL_UNSIGNED_INT_IMAGE_CUBE,
#endif
#ifdef GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY
    unsigned_int_image_cube_map_array = GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY,
#endif
#ifdef GL_UNSIGNED_INT_SAMPLER_1D
    unsigned_int_sampler_1d = GL_UNSIGNED_INT_SAMPLER_1D,
#endif
#ifdef GL_UNSIGNED_INT_SAMPLER_1D_ARRAY
    unsigned_int_sampler_1d_array = GL_UNSIGNED_INT_SAMPLER_1D_ARRAY,
#endif
#ifdef GL_UNSIGNED_INT_SAMPLER_2D
    unsigned_int_sampler_2d = GL_UNSIGNED_INT_SAMPLER_2D,
#endif
#ifdef GL_UNSIGNED_INT_SAMPLER_2D_ARRAY
    unsigned_int_sampler_2d_array = GL_UNSIGNED_INT_SAMPLER_2D_ARRAY,
#endif
#ifdef GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE
    unsigned_int_sampler_2d_multisample =
        GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE,
#endif
#ifdef GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
    unsigned_int_sampler_2d_multisample_array =
        GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
#endif
#ifdef GL_UNSIGNED_INT_SAMPLER_2D_RECT
    unsigned_int_sampler_2d_rect = GL_UNSIGNED_INT_SAMPLER_2D_RECT,
#endif
#ifdef GL_UNSIGNED_INT_SAMPLER_3D
    unsigned_int_sampler_3d = GL_UNSIGNED_INT_SAMPLER_3D,
#endif
#ifdef GL_UNSIGNED_INT_SAMPLER_BUFFER
    unsigned_int_sampler_buffer = GL_UNSIGNED_INT_SAMPLER_BUFFER,
#endif
#ifdef GL_UNSIGNED_INT_SAMPLER_CUBE
    unsigned_int_sampler_cube = GL_UNSIGNED_INT_SAMPLER_CUBE,
#endif
#ifdef GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY
    unsigned_int_sampler_cube_map_array =
        GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY,
#endif
#ifdef GL_UNSIGNED_INT_VEC2
    unsigned_int_vec2 = GL_UNSIGNED_INT_VEC2,
#endif
#ifdef GL_UNSIGNED_INT_VEC3
    unsigned_int_vec3 = GL_UNSIGNED_INT_VEC3,
#endif
#ifdef GL_UNSIGNED_INT_VEC4
    unsigned_int_vec4 = GL_UNSIGNED_INT_VEC4,
#endif
}; // enum class attribute_type

} // namespace gl::group
