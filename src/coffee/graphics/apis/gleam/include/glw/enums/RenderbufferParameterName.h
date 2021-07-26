#pragma once

#include "common.h"

namespace gl::group {

// RenderbufferParameterName
enum class renderbuffer_parameter_name : ::libc_types::u32
{
#ifdef GL_RENDERBUFFER_ALPHA_SIZE
    renderbuffer_alpha_size = GL_RENDERBUFFER_ALPHA_SIZE,
#endif
#ifdef GL_RENDERBUFFER_ALPHA_SIZE_EXT
    renderbuffer_alpha_size_ext = GL_RENDERBUFFER_ALPHA_SIZE_EXT,
#endif
#ifdef GL_RENDERBUFFER_ALPHA_SIZE_OES
    renderbuffer_alpha_size_oes = GL_RENDERBUFFER_ALPHA_SIZE_OES,
#endif
#ifdef GL_RENDERBUFFER_BLUE_SIZE
    renderbuffer_blue_size = GL_RENDERBUFFER_BLUE_SIZE,
#endif
#ifdef GL_RENDERBUFFER_BLUE_SIZE_EXT
    renderbuffer_blue_size_ext = GL_RENDERBUFFER_BLUE_SIZE_EXT,
#endif
#ifdef GL_RENDERBUFFER_BLUE_SIZE_OES
    renderbuffer_blue_size_oes = GL_RENDERBUFFER_BLUE_SIZE_OES,
#endif
#ifdef GL_RENDERBUFFER_COLOR_SAMPLES_NV
    renderbuffer_color_samples_nv = GL_RENDERBUFFER_COLOR_SAMPLES_NV,
#endif
#ifdef GL_RENDERBUFFER_COVERAGE_SAMPLES_NV
    renderbuffer_coverage_samples_nv = GL_RENDERBUFFER_COVERAGE_SAMPLES_NV,
#endif
#ifdef GL_RENDERBUFFER_DEPTH_SIZE
    renderbuffer_depth_size = GL_RENDERBUFFER_DEPTH_SIZE,
#endif
#ifdef GL_RENDERBUFFER_DEPTH_SIZE_EXT
    renderbuffer_depth_size_ext = GL_RENDERBUFFER_DEPTH_SIZE_EXT,
#endif
#ifdef GL_RENDERBUFFER_DEPTH_SIZE_OES
    renderbuffer_depth_size_oes = GL_RENDERBUFFER_DEPTH_SIZE_OES,
#endif
#ifdef GL_RENDERBUFFER_GREEN_SIZE
    renderbuffer_green_size = GL_RENDERBUFFER_GREEN_SIZE,
#endif
#ifdef GL_RENDERBUFFER_GREEN_SIZE_EXT
    renderbuffer_green_size_ext = GL_RENDERBUFFER_GREEN_SIZE_EXT,
#endif
#ifdef GL_RENDERBUFFER_GREEN_SIZE_OES
    renderbuffer_green_size_oes = GL_RENDERBUFFER_GREEN_SIZE_OES,
#endif
#ifdef GL_RENDERBUFFER_HEIGHT
    renderbuffer_height = GL_RENDERBUFFER_HEIGHT,
#endif
#ifdef GL_RENDERBUFFER_HEIGHT_EXT
    renderbuffer_height_ext = GL_RENDERBUFFER_HEIGHT_EXT,
#endif
#ifdef GL_RENDERBUFFER_HEIGHT_OES
    renderbuffer_height_oes = GL_RENDERBUFFER_HEIGHT_OES,
#endif
#ifdef GL_RENDERBUFFER_INTERNAL_FORMAT
    renderbuffer_internal_format = GL_RENDERBUFFER_INTERNAL_FORMAT,
#endif
#ifdef GL_RENDERBUFFER_INTERNAL_FORMAT_EXT
    renderbuffer_internal_format_ext = GL_RENDERBUFFER_INTERNAL_FORMAT_EXT,
#endif
#ifdef GL_RENDERBUFFER_INTERNAL_FORMAT_OES
    renderbuffer_internal_format_oes = GL_RENDERBUFFER_INTERNAL_FORMAT_OES,
#endif
#ifdef GL_RENDERBUFFER_RED_SIZE
    renderbuffer_red_size = GL_RENDERBUFFER_RED_SIZE,
#endif
#ifdef GL_RENDERBUFFER_RED_SIZE_EXT
    renderbuffer_red_size_ext = GL_RENDERBUFFER_RED_SIZE_EXT,
#endif
#ifdef GL_RENDERBUFFER_RED_SIZE_OES
    renderbuffer_red_size_oes = GL_RENDERBUFFER_RED_SIZE_OES,
#endif
#ifdef GL_RENDERBUFFER_SAMPLES
    renderbuffer_samples = GL_RENDERBUFFER_SAMPLES,
#endif
#ifdef GL_RENDERBUFFER_SAMPLES_ANGLE
    renderbuffer_samples_angle = GL_RENDERBUFFER_SAMPLES_ANGLE,
#endif
#ifdef GL_RENDERBUFFER_SAMPLES_APPLE
    renderbuffer_samples_apple = GL_RENDERBUFFER_SAMPLES_APPLE,
#endif
#ifdef GL_RENDERBUFFER_SAMPLES_EXT
    renderbuffer_samples_ext = GL_RENDERBUFFER_SAMPLES_EXT,
#endif
#ifdef GL_RENDERBUFFER_SAMPLES_IMG
    renderbuffer_samples_img = GL_RENDERBUFFER_SAMPLES_IMG,
#endif
#ifdef GL_RENDERBUFFER_SAMPLES_NV
    renderbuffer_samples_nv = GL_RENDERBUFFER_SAMPLES_NV,
#endif
#ifdef GL_RENDERBUFFER_STENCIL_SIZE
    renderbuffer_stencil_size = GL_RENDERBUFFER_STENCIL_SIZE,
#endif
#ifdef GL_RENDERBUFFER_STENCIL_SIZE_EXT
    renderbuffer_stencil_size_ext = GL_RENDERBUFFER_STENCIL_SIZE_EXT,
#endif
#ifdef GL_RENDERBUFFER_STENCIL_SIZE_OES
    renderbuffer_stencil_size_oes = GL_RENDERBUFFER_STENCIL_SIZE_OES,
#endif
#ifdef GL_RENDERBUFFER_STORAGE_SAMPLES_AMD
    renderbuffer_storage_samples_amd = GL_RENDERBUFFER_STORAGE_SAMPLES_AMD,
#endif
#ifdef GL_RENDERBUFFER_WIDTH
    renderbuffer_width = GL_RENDERBUFFER_WIDTH,
#endif
#ifdef GL_RENDERBUFFER_WIDTH_EXT
    renderbuffer_width_ext = GL_RENDERBUFFER_WIDTH_EXT,
#endif
#ifdef GL_RENDERBUFFER_WIDTH_OES
    renderbuffer_width_oes = GL_RENDERBUFFER_WIDTH_OES,
#endif
}; // enum class renderbuffer_parameter_name

} // namespace gl::group
