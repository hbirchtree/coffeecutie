#pragma once

#include "common.h"

namespace gl::group {

// TextureLayout
enum class texture_layout : u32
{
#ifdef GL_LAYOUT_COLOR_ATTACHMENT_EXT
    layout_color_attachment_ext = GL_LAYOUT_COLOR_ATTACHMENT_EXT,
#endif
#ifdef GL_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_EXT
    layout_depth_attachment_stencil_read_only_ext =
        GL_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_EXT,
#endif
#ifdef GL_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_EXT
    layout_depth_read_only_stencil_attachment_ext =
        GL_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_EXT,
#endif
#ifdef GL_LAYOUT_DEPTH_STENCIL_ATTACHMENT_EXT
    layout_depth_stencil_attachment_ext =
        GL_LAYOUT_DEPTH_STENCIL_ATTACHMENT_EXT,
#endif
#ifdef GL_LAYOUT_DEPTH_STENCIL_READ_ONLY_EXT
    layout_depth_stencil_read_only_ext = GL_LAYOUT_DEPTH_STENCIL_READ_ONLY_EXT,
#endif
#ifdef GL_LAYOUT_GENERAL_EXT
    layout_general_ext = GL_LAYOUT_GENERAL_EXT,
#endif
#ifdef GL_LAYOUT_SHADER_READ_ONLY_EXT
    layout_shader_read_only_ext = GL_LAYOUT_SHADER_READ_ONLY_EXT,
#endif
#ifdef GL_LAYOUT_TRANSFER_DST_EXT
    layout_transfer_dst_ext = GL_LAYOUT_TRANSFER_DST_EXT,
#endif
#ifdef GL_LAYOUT_TRANSFER_SRC_EXT
    layout_transfer_src_ext = GL_LAYOUT_TRANSFER_SRC_EXT,
#endif
}; // enum class texture_layout

} // namespace gl::group
