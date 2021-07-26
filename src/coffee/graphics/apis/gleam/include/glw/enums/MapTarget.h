#pragma once

#include "common.h"

namespace gl::group {

// MapTarget
enum class map_target : ::libc_types::u32
{
#ifdef GL_GEOMETRY_DEFORMATION_SGIX
    geometry_deformation_sgix = GL_GEOMETRY_DEFORMATION_SGIX,
#endif
#ifdef GL_MAP1_COLOR_4
    map1_color_4 = GL_MAP1_COLOR_4,
#endif
#ifdef GL_MAP1_INDEX
    map1_index = GL_MAP1_INDEX,
#endif
#ifdef GL_MAP1_NORMAL
    map1_normal = GL_MAP1_NORMAL,
#endif
#ifdef GL_MAP1_TEXTURE_COORD_1
    map1_texture_coord_1 = GL_MAP1_TEXTURE_COORD_1,
#endif
#ifdef GL_MAP1_TEXTURE_COORD_2
    map1_texture_coord_2 = GL_MAP1_TEXTURE_COORD_2,
#endif
#ifdef GL_MAP1_TEXTURE_COORD_3
    map1_texture_coord_3 = GL_MAP1_TEXTURE_COORD_3,
#endif
#ifdef GL_MAP1_TEXTURE_COORD_4
    map1_texture_coord_4 = GL_MAP1_TEXTURE_COORD_4,
#endif
#ifdef GL_MAP1_VERTEX_3
    map1_vertex_3 = GL_MAP1_VERTEX_3,
#endif
#ifdef GL_MAP1_VERTEX_4
    map1_vertex_4 = GL_MAP1_VERTEX_4,
#endif
#ifdef GL_MAP2_COLOR_4
    map2_color_4 = GL_MAP2_COLOR_4,
#endif
#ifdef GL_MAP2_INDEX
    map2_index = GL_MAP2_INDEX,
#endif
#ifdef GL_MAP2_NORMAL
    map2_normal = GL_MAP2_NORMAL,
#endif
#ifdef GL_MAP2_TEXTURE_COORD_1
    map2_texture_coord_1 = GL_MAP2_TEXTURE_COORD_1,
#endif
#ifdef GL_MAP2_TEXTURE_COORD_2
    map2_texture_coord_2 = GL_MAP2_TEXTURE_COORD_2,
#endif
#ifdef GL_MAP2_TEXTURE_COORD_3
    map2_texture_coord_3 = GL_MAP2_TEXTURE_COORD_3,
#endif
#ifdef GL_MAP2_TEXTURE_COORD_4
    map2_texture_coord_4 = GL_MAP2_TEXTURE_COORD_4,
#endif
#ifdef GL_MAP2_VERTEX_3
    map2_vertex_3 = GL_MAP2_VERTEX_3,
#endif
#ifdef GL_MAP2_VERTEX_4
    map2_vertex_4 = GL_MAP2_VERTEX_4,
#endif
#ifdef GL_TEXTURE_DEFORMATION_SGIX
    texture_deformation_sgix = GL_TEXTURE_DEFORMATION_SGIX,
#endif
}; // enum class map_target

} // namespace gl::group
