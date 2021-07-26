#pragma once

#include "common.h"

namespace gl::group {

// FramebufferParameterName
enum class framebuffer_parameter_name : ::libc_types::u32
{
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
}; // enum class framebuffer_parameter_name

} // namespace gl::group
