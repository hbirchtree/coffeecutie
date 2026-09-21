#pragma once

#include "common.h"

namespace gl::group {

// DepthStencilTextureMode
enum class depth_stencil_texture_mode : u32
{
    depth_component = 0x1902, // GL_DEPTH_COMPONENT
    stencil_index   = 0x1901, // GL_STENCIL_INDEX
}; // enum class depth_stencil_texture_mode

} // namespace gl::group
