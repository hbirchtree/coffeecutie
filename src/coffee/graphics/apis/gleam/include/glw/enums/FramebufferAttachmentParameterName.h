#pragma once

#include "common.h"

namespace gl::group {

// FramebufferAttachmentParameterName
enum class framebuffer_attachment_parameter_name : u32
{
#ifdef GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE
    framebuffer_attachment_alpha_size = GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE
    framebuffer_attachment_blue_size = GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING
    framebuffer_attachment_color_encoding =
        GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING_EXT
    framebuffer_attachment_color_encoding_ext =
        GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE
    framebuffer_attachment_component_type =
        GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE_EXT
    framebuffer_attachment_component_type_ext =
        GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE
    framebuffer_attachment_depth_size = GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE
    framebuffer_attachment_green_size = GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_LAYERED
    framebuffer_attachment_layered = GL_FRAMEBUFFER_ATTACHMENT_LAYERED,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_LAYERED_ARB
    framebuffer_attachment_layered_arb = GL_FRAMEBUFFER_ATTACHMENT_LAYERED_ARB,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_LAYERED_EXT
    framebuffer_attachment_layered_ext = GL_FRAMEBUFFER_ATTACHMENT_LAYERED_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_LAYERED_OES
    framebuffer_attachment_layered_oes = GL_FRAMEBUFFER_ATTACHMENT_LAYERED_OES,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME
    framebuffer_attachment_object_name = GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT
    framebuffer_attachment_object_name_ext =
        GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_OES
    framebuffer_attachment_object_name_oes =
        GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_OES,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE
    framebuffer_attachment_object_type = GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT
    framebuffer_attachment_object_type_ext =
        GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_OES
    framebuffer_attachment_object_type_oes =
        GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_OES,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE
    framebuffer_attachment_red_size = GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE
    framebuffer_attachment_stencil_size =
        GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT
    framebuffer_attachment_texture_3d_zoffset_ext =
        GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_OES
    framebuffer_attachment_texture_3d_zoffset_oes =
        GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_OES,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_BASE_VIEW_INDEX_OVR
    framebuffer_attachment_texture_base_view_index_ovr =
        GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_BASE_VIEW_INDEX_OVR,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE
    framebuffer_attachment_texture_cube_map_face =
        GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT
    framebuffer_attachment_texture_cube_map_face_ext =
        GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_OES
    framebuffer_attachment_texture_cube_map_face_oes =
        GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_OES,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER
    framebuffer_attachment_texture_layer =
        GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER_EXT
    framebuffer_attachment_texture_layer_ext =
        GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL
    framebuffer_attachment_texture_level =
        GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT
    framebuffer_attachment_texture_level_ext =
        GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_OES
    framebuffer_attachment_texture_level_oes =
        GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_OES,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR
    framebuffer_attachment_texture_num_views_ovr =
        GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_SAMPLES_EXT
    framebuffer_attachment_texture_samples_ext =
        GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_SAMPLES_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_SCALE_IMG
    framebuffer_attachment_texture_scale_img =
        GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_SCALE_IMG,
#endif
}; // enum class framebuffer_attachment_parameter_name

} // namespace gl::group
