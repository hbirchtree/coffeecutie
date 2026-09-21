#pragma once

#include "common.h"

namespace gl::group {

// RenderbufferParameterName
enum class renderbuffer_parameter_name : u32
{
    renderbuffer_alpha_size       = 0x8D53, // GL_RENDERBUFFER_ALPHA_SIZE
    renderbuffer_blue_size        = 0x8D52, // GL_RENDERBUFFER_BLUE_SIZE
    renderbuffer_depth_size       = 0x8D54, // GL_RENDERBUFFER_DEPTH_SIZE
    renderbuffer_green_size       = 0x8D51, // GL_RENDERBUFFER_GREEN_SIZE
    renderbuffer_height           = 0x8D43, // GL_RENDERBUFFER_HEIGHT
    renderbuffer_internal_format  = 0x8D44, // GL_RENDERBUFFER_INTERNAL_FORMAT
    renderbuffer_red_size         = 0x8D50, // GL_RENDERBUFFER_RED_SIZE
    renderbuffer_samples          = 0x8CAB, // GL_RENDERBUFFER_SAMPLES
    renderbuffer_stencil_size     = 0x8D55, // GL_RENDERBUFFER_STENCIL_SIZE
    renderbuffer_width            = 0x8D42, // GL_RENDERBUFFER_WIDTH
    renderbuffer_alpha_size_ext   = 0x8D53, // GL_RENDERBUFFER_ALPHA_SIZE_EXT
    renderbuffer_alpha_size_oes   = 0x8D53, // GL_RENDERBUFFER_ALPHA_SIZE_OES
    renderbuffer_blue_size_ext    = 0x8D52, // GL_RENDERBUFFER_BLUE_SIZE_EXT
    renderbuffer_blue_size_oes    = 0x8D52, // GL_RENDERBUFFER_BLUE_SIZE_OES
    renderbuffer_color_samples_nv = 0x8E10, // GL_RENDERBUFFER_COLOR_SAMPLES_NV
    renderbuffer_coverage_samples_nv =
        0x8CAB,                           // GL_RENDERBUFFER_COVERAGE_SAMPLES_NV
    renderbuffer_depth_size_ext = 0x8D54, // GL_RENDERBUFFER_DEPTH_SIZE_EXT
    renderbuffer_depth_size_oes = 0x8D54, // GL_RENDERBUFFER_DEPTH_SIZE_OES
    renderbuffer_green_size_ext = 0x8D51, // GL_RENDERBUFFER_GREEN_SIZE_EXT
    renderbuffer_green_size_oes = 0x8D51, // GL_RENDERBUFFER_GREEN_SIZE_OES
    renderbuffer_height_ext     = 0x8D43, // GL_RENDERBUFFER_HEIGHT_EXT
    renderbuffer_height_oes     = 0x8D43, // GL_RENDERBUFFER_HEIGHT_OES
    renderbuffer_internal_format_ext =
        0x8D44, // GL_RENDERBUFFER_INTERNAL_FORMAT_EXT
    renderbuffer_internal_format_oes =
        0x8D44, // GL_RENDERBUFFER_INTERNAL_FORMAT_OES
    renderbuffer_red_size_ext     = 0x8D50, // GL_RENDERBUFFER_RED_SIZE_EXT
    renderbuffer_red_size_oes     = 0x8D50, // GL_RENDERBUFFER_RED_SIZE_OES
    renderbuffer_samples_angle    = 0x8CAB, // GL_RENDERBUFFER_SAMPLES_ANGLE
    renderbuffer_samples_apple    = 0x8CAB, // GL_RENDERBUFFER_SAMPLES_APPLE
    renderbuffer_samples_ext      = 0x8CAB, // GL_RENDERBUFFER_SAMPLES_EXT
    renderbuffer_samples_img      = 0x9133, // GL_RENDERBUFFER_SAMPLES_IMG
    renderbuffer_samples_nv       = 0x8CAB, // GL_RENDERBUFFER_SAMPLES_NV
    renderbuffer_stencil_size_ext = 0x8D55, // GL_RENDERBUFFER_STENCIL_SIZE_EXT
    renderbuffer_stencil_size_oes = 0x8D55, // GL_RENDERBUFFER_STENCIL_SIZE_OES
    renderbuffer_storage_samples_amd =
        0x91B2,                      // GL_RENDERBUFFER_STORAGE_SAMPLES_AMD
    renderbuffer_width_ext = 0x8D42, // GL_RENDERBUFFER_WIDTH_EXT
    renderbuffer_width_oes = 0x8D42, // GL_RENDERBUFFER_WIDTH_OES
}; // enum class renderbuffer_parameter_name

} // namespace gl::group
