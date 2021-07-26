#pragma once

#include "common.h"

namespace gl::group {

// GetFramebufferParameter
enum class get_framebuffer_parameter : ::libc_types::u32
{
#ifdef GL_DOUBLEBUFFER
    doublebuffer = GL_DOUBLEBUFFER,
#endif
#ifdef GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS
    framebuffer_default_fixed_sample_locations =
        GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS,
#endif
#ifdef GL_FRAMEBUFFER_DEFAULT_HEIGHT
    framebuffer_default_height = GL_FRAMEBUFFER_DEFAULT_HEIGHT,
#endif
#ifdef GL_FRAMEBUFFER_DEFAULT_LAYERS
    framebuffer_default_layers = GL_FRAMEBUFFER_DEFAULT_LAYERS,
#endif
#ifdef GL_FRAMEBUFFER_DEFAULT_SAMPLES
    framebuffer_default_samples = GL_FRAMEBUFFER_DEFAULT_SAMPLES,
#endif
#ifdef GL_FRAMEBUFFER_DEFAULT_WIDTH
    framebuffer_default_width = GL_FRAMEBUFFER_DEFAULT_WIDTH,
#endif
#ifdef GL_IMPLEMENTATION_COLOR_READ_FORMAT
    implementation_color_read_format = GL_IMPLEMENTATION_COLOR_READ_FORMAT,
#endif
#ifdef GL_IMPLEMENTATION_COLOR_READ_TYPE
    implementation_color_read_type = GL_IMPLEMENTATION_COLOR_READ_TYPE,
#endif
#ifdef GL_SAMPLES
    samples = GL_SAMPLES,
#endif
#ifdef GL_SAMPLE_BUFFERS
    sample_buffers = GL_SAMPLE_BUFFERS,
#endif
#ifdef GL_STEREO
    stereo = GL_STEREO,
#endif
}; // enum class get_framebuffer_parameter

} // namespace gl::group
