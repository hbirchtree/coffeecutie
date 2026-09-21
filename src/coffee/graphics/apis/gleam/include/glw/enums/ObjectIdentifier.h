#pragma once

#include "common.h"

namespace gl::group {

// ObjectIdentifier
enum class object_identifier : u32
{
    buffer             = 0x82E0, // GL_BUFFER
    framebuffer        = 0x8D40, // GL_FRAMEBUFFER
    program            = 0x82E2, // GL_PROGRAM
    program_pipeline   = 0x82E4, // GL_PROGRAM_PIPELINE
    query              = 0x82E3, // GL_QUERY
    renderbuffer       = 0x8D41, // GL_RENDERBUFFER
    sampler            = 0x82E6, // GL_SAMPLER
    shader             = 0x82E1, // GL_SHADER
    texture            = 0x1702, // GL_TEXTURE
    transform_feedback = 0x8E22, // GL_TRANSFORM_FEEDBACK
    vertex_array       = 0x8074, // GL_VERTEX_ARRAY
}; // enum class object_identifier

} // namespace gl::group
