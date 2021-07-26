#pragma once

#include "common.h"

namespace gl::group {

// GlslTypeToken
enum class glsl_type_token : ::libc_types::u32
{
#ifdef GL_BOOL
    bool_ = GL_BOOL,
#endif
#ifdef GL_BOOL_VEC2
    bool_vec2 = GL_BOOL_VEC2,
#endif
#ifdef GL_BOOL_VEC3
    bool_vec3 = GL_BOOL_VEC3,
#endif
#ifdef GL_BOOL_VEC4
    bool_vec4 = GL_BOOL_VEC4,
#endif
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
#ifdef GL_DOUBLE_MAT2
    double_mat2 = GL_DOUBLE_MAT2,
#endif
#ifdef GL_DOUBLE_MAT3
    double_mat3 = GL_DOUBLE_MAT3,
#endif
#ifdef GL_DOUBLE_MAT4
    double_mat4 = GL_DOUBLE_MAT4,
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
#ifdef GL_FLOAT_MAT2x3
    float_mat2x3 = GL_FLOAT_MAT2x3,
#endif
#ifdef GL_FLOAT_MAT2x4
    float_mat2x4 = GL_FLOAT_MAT2x4,
#endif
#ifdef GL_FLOAT_MAT3
    float_mat3 = GL_FLOAT_MAT3,
#endif
#ifdef GL_FLOAT_MAT3x2
    float_mat3x2 = GL_FLOAT_MAT3x2,
#endif
#ifdef GL_FLOAT_MAT3x4
    float_mat3x4 = GL_FLOAT_MAT3x4,
#endif
#ifdef GL_FLOAT_MAT4
    float_mat4 = GL_FLOAT_MAT4,
#endif
#ifdef GL_FLOAT_MAT4x2
    float_mat4x2 = GL_FLOAT_MAT4x2,
#endif
#ifdef GL_FLOAT_MAT4x3
    float_mat4x3 = GL_FLOAT_MAT4x3,
#endif
#ifdef GL_FLOAT_VEC2
    float_vec2 = GL_FLOAT_VEC2,
#endif
#ifdef GL_FLOAT_VEC3
    float_vec3 = GL_FLOAT_VEC3,
#endif
#ifdef GL_FLOAT_VEC4
    float_vec4 = GL_FLOAT_VEC4,
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
#ifdef GL_INT_VEC3
    int_vec3 = GL_INT_VEC3,
#endif
#ifdef GL_INT_VEC4
    int_vec4 = GL_INT_VEC4,
#endif
#ifdef GL_SAMPLER_1D
    sampler_1d = GL_SAMPLER_1D,
#endif
#ifdef GL_SAMPLER_1D_ARRAY
    sampler_1d_array = GL_SAMPLER_1D_ARRAY,
#endif
#ifdef GL_SAMPLER_1D_ARRAY_SHADOW
    sampler_1d_array_shadow = GL_SAMPLER_1D_ARRAY_SHADOW,
#endif
#ifdef GL_SAMPLER_1D_SHADOW
    sampler_1d_shadow = GL_SAMPLER_1D_SHADOW,
#endif
#ifdef GL_SAMPLER_2D
    sampler_2d = GL_SAMPLER_2D,
#endif
#ifdef GL_SAMPLER_2D_ARRAY
    sampler_2d_array = GL_SAMPLER_2D_ARRAY,
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
#ifdef GL_SAMPLER_2D_RECT_SHADOW
    sampler_2d_rect_shadow = GL_SAMPLER_2D_RECT_SHADOW,
#endif
#ifdef GL_SAMPLER_2D_SHADOW
    sampler_2d_shadow = GL_SAMPLER_2D_SHADOW,
#endif
#ifdef GL_SAMPLER_3D
    sampler_3d = GL_SAMPLER_3D,
#endif
#ifdef GL_SAMPLER_BUFFER
    sampler_buffer = GL_SAMPLER_BUFFER,
#endif
#ifdef GL_SAMPLER_CUBE
    sampler_cube = GL_SAMPLER_CUBE,
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
#ifdef GL_UNSIGNED_INT_ATOMIC_COUNTER
    unsigned_int_atomic_counter = GL_UNSIGNED_INT_ATOMIC_COUNTER,
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
}; // enum class glsl_type_token

} // namespace gl::group
