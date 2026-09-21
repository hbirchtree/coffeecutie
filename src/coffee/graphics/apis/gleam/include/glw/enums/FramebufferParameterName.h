#pragma once

#include "common.h"

namespace gl::group {

// FramebufferParameterName
enum class framebuffer_parameter_name : u32
{
    framebuffer_default_fixed_sample_locations =
        0x9314, // GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS
    framebuffer_default_height  = 0x9311, // GL_FRAMEBUFFER_DEFAULT_HEIGHT
    framebuffer_default_layers  = 0x9312, // GL_FRAMEBUFFER_DEFAULT_LAYERS
    framebuffer_default_samples = 0x9313, // GL_FRAMEBUFFER_DEFAULT_SAMPLES
    framebuffer_default_width   = 0x9310, // GL_FRAMEBUFFER_DEFAULT_WIDTH
}; // enum class framebuffer_parameter_name

} // namespace gl::group
