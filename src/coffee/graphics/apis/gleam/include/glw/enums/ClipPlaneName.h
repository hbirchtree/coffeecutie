#pragma once

#include "common.h"

namespace gl::group {

// ClipPlaneName
enum class clip_plane_name : u32
{
#ifdef GL_CLIP_DISTANCE0
    clip_distance0 = GL_CLIP_DISTANCE0,
#endif
#ifdef GL_CLIP_DISTANCE1
    clip_distance1 = GL_CLIP_DISTANCE1,
#endif
#ifdef GL_CLIP_DISTANCE2
    clip_distance2 = GL_CLIP_DISTANCE2,
#endif
#ifdef GL_CLIP_DISTANCE3
    clip_distance3 = GL_CLIP_DISTANCE3,
#endif
#ifdef GL_CLIP_DISTANCE4
    clip_distance4 = GL_CLIP_DISTANCE4,
#endif
#ifdef GL_CLIP_DISTANCE5
    clip_distance5 = GL_CLIP_DISTANCE5,
#endif
#ifdef GL_CLIP_DISTANCE6
    clip_distance6 = GL_CLIP_DISTANCE6,
#endif
#ifdef GL_CLIP_DISTANCE7
    clip_distance7 = GL_CLIP_DISTANCE7,
#endif
#ifdef GL_CLIP_PLANE0
    clip_plane0 = GL_CLIP_PLANE0,
#endif
#ifdef GL_CLIP_PLANE1
    clip_plane1 = GL_CLIP_PLANE1,
#endif
#ifdef GL_CLIP_PLANE2
    clip_plane2 = GL_CLIP_PLANE2,
#endif
#ifdef GL_CLIP_PLANE3
    clip_plane3 = GL_CLIP_PLANE3,
#endif
#ifdef GL_CLIP_PLANE4
    clip_plane4 = GL_CLIP_PLANE4,
#endif
#ifdef GL_CLIP_PLANE5
    clip_plane5 = GL_CLIP_PLANE5,
#endif
}; // enum class clip_plane_name

} // namespace gl::group
