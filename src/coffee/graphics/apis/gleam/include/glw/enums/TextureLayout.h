#pragma once

#include "common.h"

namespace gl::group {

// TextureLayout
enum class texture_layout : u32
{
    layout_color_attachment_ext = 0x958E, // GL_LAYOUT_COLOR_ATTACHMENT_EXT
    layout_depth_attachment_stencil_read_only_ext =
        0x9531, // GL_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_EXT
    layout_depth_read_only_stencil_attachment_ext =
        0x9530, // GL_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_EXT
    layout_depth_stencil_attachment_ext =
        0x958F, // GL_LAYOUT_DEPTH_STENCIL_ATTACHMENT_EXT
    layout_depth_stencil_read_only_ext =
        0x9590, // GL_LAYOUT_DEPTH_STENCIL_READ_ONLY_EXT
    layout_general_ext          = 0x958D, // GL_LAYOUT_GENERAL_EXT
    layout_shader_read_only_ext = 0x9591, // GL_LAYOUT_SHADER_READ_ONLY_EXT
    layout_transfer_dst_ext     = 0x9593, // GL_LAYOUT_TRANSFER_DST_EXT
    layout_transfer_src_ext     = 0x9592, // GL_LAYOUT_TRANSFER_SRC_EXT
}; // enum class texture_layout

} // namespace gl::group
