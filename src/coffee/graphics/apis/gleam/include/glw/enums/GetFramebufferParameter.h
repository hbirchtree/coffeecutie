#pragma once

#include "common.h"

namespace gl::group {

// GetFramebufferParameter
enum class get_framebuffer_parameter : u32
{
    doublebuffer = 0x0C32, // GL_DOUBLEBUFFER
    framebuffer_default_fixed_sample_locations =
        0x9314, // GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS
    framebuffer_default_height  = 0x9311, // GL_FRAMEBUFFER_DEFAULT_HEIGHT
    framebuffer_default_layers  = 0x9312, // GL_FRAMEBUFFER_DEFAULT_LAYERS
    framebuffer_default_samples = 0x9313, // GL_FRAMEBUFFER_DEFAULT_SAMPLES
    framebuffer_default_width   = 0x9310, // GL_FRAMEBUFFER_DEFAULT_WIDTH
    implementation_color_read_format =
        0x8B9B, // GL_IMPLEMENTATION_COLOR_READ_FORMAT
    implementation_color_read_type =
        0x8B9A,              // GL_IMPLEMENTATION_COLOR_READ_TYPE
    samples        = 0x80A9, // GL_SAMPLES
    sample_buffers = 0x80A8, // GL_SAMPLE_BUFFERS
    stereo         = 0x0C33, // GL_STEREO
}; // enum class get_framebuffer_parameter

} // namespace gl::group
