#pragma once

#include "common.h"

namespace gl::group {

// FogPName
enum class fog_prop : u32
{
    fog_coord_src = 0x8450, // GL_FOG_COORD_SRC
    fog_density   = 0x0B62, // GL_FOG_DENSITY
    fog_end       = 0x0B64, // GL_FOG_END
    fog_index     = 0x0B61, // GL_FOG_INDEX
    fog_mode      = 0x0B65, // GL_FOG_MODE
    fog_start     = 0x0B63, // GL_FOG_START
}; // enum class fog_prop

} // namespace gl::group
