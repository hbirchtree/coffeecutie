#pragma once

#include "common.h"

namespace gl::group {

// FramebufferAttachmentParameterName
enum class framebuffer_attachment_parameter_name : u32
{
    framebuffer_attachment_alpha_size =
        0x8215, // GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE
    framebuffer_attachment_blue_size =
        0x8214, // GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE
    framebuffer_attachment_color_encoding =
        0x8210, // GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING
    framebuffer_attachment_component_type =
        0x8211, // GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE
    framebuffer_attachment_depth_size =
        0x8216, // GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE
    framebuffer_attachment_green_size =
        0x8213, // GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE
    framebuffer_attachment_layered =
        0x8DA7, // GL_FRAMEBUFFER_ATTACHMENT_LAYERED
    framebuffer_attachment_object_name =
        0x8CD1, // GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME
    framebuffer_attachment_object_type =
        0x8CD0, // GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE
    framebuffer_attachment_red_size =
        0x8212, // GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE
    framebuffer_attachment_stencil_size =
        0x8217, // GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE
    framebuffer_attachment_texture_cube_map_face =
        0x8CD3, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE
    framebuffer_attachment_texture_layer =
        0x8CD4, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER
    framebuffer_attachment_texture_level =
        0x8CD2, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL
    framebuffer_attachment_color_encoding_ext =
        0x8210, // GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING_EXT
    framebuffer_attachment_component_type_ext =
        0x8211, // GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE_EXT
    framebuffer_attachment_layered_arb =
        0x8DA7, // GL_FRAMEBUFFER_ATTACHMENT_LAYERED_ARB
    framebuffer_attachment_layered_ext =
        0x8DA7, // GL_FRAMEBUFFER_ATTACHMENT_LAYERED_EXT
    framebuffer_attachment_layered_oes =
        0x8DA7, // GL_FRAMEBUFFER_ATTACHMENT_LAYERED_OES
    framebuffer_attachment_object_name_ext =
        0x8CD1, // GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT
    framebuffer_attachment_object_name_oes =
        0x8CD1, // GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_OES
    framebuffer_attachment_object_type_ext =
        0x8CD0, // GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT
    framebuffer_attachment_object_type_oes =
        0x8CD0, // GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_OES
    framebuffer_attachment_texture_3d_zoffset_ext =
        0x8CD4, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT
    framebuffer_attachment_texture_3d_zoffset_oes =
        0x8CD4, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_OES
    framebuffer_attachment_texture_base_view_index_ovr =
        0x9632, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_BASE_VIEW_INDEX_OVR
    framebuffer_attachment_texture_cube_map_face_ext =
        0x8CD3, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT
    framebuffer_attachment_texture_cube_map_face_oes =
        0x8CD3, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_OES
    framebuffer_attachment_texture_layer_ext =
        0x8CD4, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER_EXT
    framebuffer_attachment_texture_level_ext =
        0x8CD2, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT
    framebuffer_attachment_texture_level_oes =
        0x8CD2, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_OES
    framebuffer_attachment_texture_num_views_ovr =
        0x9630, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR
    framebuffer_attachment_texture_samples_ext =
        0x8D6C, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_SAMPLES_EXT
    framebuffer_attachment_texture_scale_img =
        0x913F, // GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_SCALE_IMG
}; // enum class framebuffer_attachment_parameter_name

} // namespace gl::group
