#pragma once

#include "common.h"

namespace gl::group {

// MapTarget
enum class map_target : u32
{
    map1_color_4              = 0x0D90, // GL_MAP1_COLOR_4
    map1_index                = 0x0D91, // GL_MAP1_INDEX
    map1_normal               = 0x0D92, // GL_MAP1_NORMAL
    map1_texture_coord_1      = 0x0D93, // GL_MAP1_TEXTURE_COORD_1
    map1_texture_coord_2      = 0x0D94, // GL_MAP1_TEXTURE_COORD_2
    map1_texture_coord_3      = 0x0D95, // GL_MAP1_TEXTURE_COORD_3
    map1_texture_coord_4      = 0x0D96, // GL_MAP1_TEXTURE_COORD_4
    map1_vertex_3             = 0x0D97, // GL_MAP1_VERTEX_3
    map1_vertex_4             = 0x0D98, // GL_MAP1_VERTEX_4
    map2_color_4              = 0x0DB0, // GL_MAP2_COLOR_4
    map2_index                = 0x0DB1, // GL_MAP2_INDEX
    map2_normal               = 0x0DB2, // GL_MAP2_NORMAL
    map2_texture_coord_1      = 0x0DB3, // GL_MAP2_TEXTURE_COORD_1
    map2_texture_coord_2      = 0x0DB4, // GL_MAP2_TEXTURE_COORD_2
    map2_texture_coord_3      = 0x0DB5, // GL_MAP2_TEXTURE_COORD_3
    map2_texture_coord_4      = 0x0DB6, // GL_MAP2_TEXTURE_COORD_4
    map2_vertex_3             = 0x0DB7, // GL_MAP2_VERTEX_3
    map2_vertex_4             = 0x0DB8, // GL_MAP2_VERTEX_4
    geometry_deformation_sgix = 0x8194, // GL_GEOMETRY_DEFORMATION_SGIX
    texture_deformation_sgix  = 0x8195, // GL_TEXTURE_DEFORMATION_SGIX
}; // enum class map_target

} // namespace gl::group
