#pragma once

#include "common.h"

namespace gl::groups {

// ObjectIdentifier
enum class object_identifier : ::libc_types::u32 {
#ifdef GL_BUFFER
    buffer = GL_BUFFER,
#endif
#ifdef GL_FRAMEBUFFER
    framebuffer = GL_FRAMEBUFFER,
#endif
#ifdef GL_PROGRAM
    program = GL_PROGRAM,
#endif
#ifdef GL_PROGRAM_PIPELINE
    program_pipeline = GL_PROGRAM_PIPELINE,
#endif
#ifdef GL_QUERY
    query = GL_QUERY,
#endif
#ifdef GL_RENDERBUFFER
    renderbuffer = GL_RENDERBUFFER,
#endif
#ifdef GL_SAMPLER
    sampler = GL_SAMPLER,
#endif
#ifdef GL_SHADER
    shader = GL_SHADER,
#endif
#ifdef GL_TEXTURE
    texture = GL_TEXTURE,
#endif
#ifdef GL_TRANSFORM_FEEDBACK
    transform_feedback = GL_TRANSFORM_FEEDBACK,
#endif
#ifdef GL_VERTEX_ARRAY
    vertex_array = GL_VERTEX_ARRAY,
#endif
}; // enum class object_identifier

} // namespace gl::groups
