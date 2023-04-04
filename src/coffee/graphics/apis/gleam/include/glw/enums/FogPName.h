#pragma once

#include "common.h"

namespace gl::group {

// FogPName
enum class fog_prop : u32
{
#ifdef GL_FOG_COORD_SRC
    fog_coord_src = GL_FOG_COORD_SRC,
#endif
#ifdef GL_FOG_DENSITY
    fog_density = GL_FOG_DENSITY,
#endif
#ifdef GL_FOG_END
    fog_end = GL_FOG_END,
#endif
#ifdef GL_FOG_INDEX
    fog_index = GL_FOG_INDEX,
#endif
#ifdef GL_FOG_MODE
    fog_mode = GL_FOG_MODE,
#endif
#ifdef GL_FOG_START
    fog_start = GL_FOG_START,
#endif
}; // enum class fog_prop

} // namespace gl::group
