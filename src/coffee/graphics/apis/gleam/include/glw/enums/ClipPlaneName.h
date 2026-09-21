#pragma once

#include "common.h"

namespace gl::group {

// ClipPlaneName
enum class clip_plane_name : u32
{
    clip_distance0 = 0x3000, // GL_CLIP_DISTANCE0
    clip_distance1 = 0x3001, // GL_CLIP_DISTANCE1
    clip_distance2 = 0x3002, // GL_CLIP_DISTANCE2
    clip_distance3 = 0x3003, // GL_CLIP_DISTANCE3
    clip_distance4 = 0x3004, // GL_CLIP_DISTANCE4
    clip_distance5 = 0x3005, // GL_CLIP_DISTANCE5
    clip_distance6 = 0x3006, // GL_CLIP_DISTANCE6
    clip_distance7 = 0x3007, // GL_CLIP_DISTANCE7
    clip_plane0    = 0x3000, // GL_CLIP_PLANE0
    clip_plane1    = 0x3001, // GL_CLIP_PLANE1
    clip_plane2    = 0x3002, // GL_CLIP_PLANE2
    clip_plane3    = 0x3003, // GL_CLIP_PLANE3
    clip_plane4    = 0x3004, // GL_CLIP_PLANE4
    clip_plane5    = 0x3005, // GL_CLIP_PLANE5
}; // enum class clip_plane_name

} // namespace gl::group
