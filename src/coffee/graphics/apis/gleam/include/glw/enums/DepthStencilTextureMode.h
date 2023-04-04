#pragma once

#include "common.h"

namespace gl::group {

// DepthStencilTextureMode
enum class depth_stencil_texture_mode : u32
{
#ifdef GL_DEPTH_COMPONENT
    depth_component = GL_DEPTH_COMPONENT,
#endif
#ifdef GL_STENCIL_INDEX
    stencil_index = GL_STENCIL_INDEX,
#endif
}; // enum class depth_stencil_texture_mode

} // namespace gl::group
